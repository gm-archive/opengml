#include "library.h"
#include "ogm/interpreter/Variable.hpp"
#include "ogm/common/error.hpp"
#include "ogm/common/util.hpp"
#include "ogm/interpreter/Executor.hpp"
#include "ogm/interpreter/execute.hpp"

#ifdef PELOADER
extern "C"
{
#include "fn_external_c.h"
}
#endif

#ifdef EMBED_ZUGBRUECKE
#include <Python.h>
#include "ogm/interpreter/Debugger.hpp"
#endif

#if defined(_WIN32) || defined(WIN32)
// this seems to be necessary to use the SetDllDirectoryA function.
#define _WIN32_WINNT 0x0502
#include <winbase.h>
// include moved here otherwise we get ambiguous function call errors for _InterlockedExchange
#include <windows.h>
#endif

#include <string>
#include "ogm/common/error.hpp"
#include <locale>
#include <cctype>
#include <cstdlib>

using namespace ogm::interpreter;
using namespace ogm::interpreter::fn;

namespace
{

enum class CallType
{
    _CDECL,
    _STDCALL
};

typedef uint32_t external_id_t;

char sig_char(VariableType vc)
{
    if (vc == VT_REAL) return 'r';
    else if (vc == VT_STRING) return 's';
    else throw MiscError("Variable type for external call must be either string or real.");
}

#include "fn_external_call_def.h"

// forward declarations
external_id_t external_define_impl(const char* path, const char* fnname, CallType, VariableType rettype, uint32_t argc, VariableType* argtype);
void external_call_impl(VO out, external_id_t, byte argc,  const Variable* argv);
void external_free_impl(external_id_t);

#ifdef __unix__
#define RESOLVED

#include <dlfcn.h>

DEFN_external_call(, void*)

struct ExternalDefinitionDL
{
    void* m_dl;
    std::string m_dl_path;
    void* m_dll_fn_address;
    CallType m_ct;
    std::string m_sig;

    #ifdef EMBED_ZUGBRUECKE
    bool m_zugbruecke = false;
    #endif
};

std::map<external_id_t, ExternalDefinitionDL> g_dlls;
std::map<std::string, void*> g_path_to_dll;
std::map<void*, size_t> g_dll_refc;

inline external_id_t get_next_id()
{
    external_id_t i = 0;
    while (g_dlls.find(i) != g_dlls.end())
    {
        ++i;
    }
    ogm_assert(g_dlls.find(i) == g_dlls.end());
    return i;
}


#ifdef EMBED_ZUGBRUECKE
bool g_zugbruecke_setup_complete = false;
bool g_zugbruecke_available;

// zugbruecke module
PyObject* g_zugbruecke = nullptr;
PyObject* g_zugbruecke_cdecl = nullptr;
PyObject* g_zugbruecke_stdcall = nullptr;

bool zugbruecke_init()
{
    if (g_zugbruecke_setup_complete)
    {
        return g_zugbruecke_available;
    }

    std::cout << "Initializing zugbruecke (for win32 DLL)..." << std::endl;

    g_zugbruecke_setup_complete = true;
    g_zugbruecke_available = false;

    // setup python
    Py_Initialize();

    // TODO: should be cleaned up witha call to Py_Finalize().

    PyObject *pName;

    pName = PyUnicode_DecodeFSDefault("zugbruecke");

    g_zugbruecke = PyImport_Import(pName);
    Py_DECREF(pName);

    if (!g_zugbruecke)
    {
        std::cerr << "Failed to load module zugbruecke for running win32 dll.\n";
        std::cerr << "Try running `python3 -m pip install zugbruecke`.\n";
        return false;
    }

    PyObject* cdll = PyObject_GetAttrString(g_zugbruecke, "cdll");
    PyObject* windll = PyObject_GetAttrString(g_zugbruecke, "windll");

    if (cdll)
    {
        g_zugbruecke_cdecl = PyObject_GetAttrString(cdll, "LoadLibrary");
        Py_DECREF(cdll);
    }

    if (windll)
    {
        g_zugbruecke_stdcall = PyObject_GetAttrString(windll, "LoadLibrary");
        Py_DECREF(windll);
    }

    g_zugbruecke_available = true;

    std::cout << "...zugbruecke initialized." << std::endl;

    return true;
}

external_id_t external_define_zugbruecke_impl(const char* path, const char* fnname, CallType ct, VariableType rettype, uint32_t argc, VariableType* argt)
{
    ExternalDefinitionDL ed;
    ed.m_ct = ct;
    ed.m_sig.push_back(sig_char(rettype));
    ed.m_dl_path = path;
    ed.m_dl = nullptr;
    ed.m_zugbruecke = true;
    for (size_t i = 0; i < argc; ++i)
    {
        ed.m_sig.push_back(sig_char(argt[i]));
    }

    if (g_path_to_dll.find(path) == g_path_to_dll.end())
    {
        PyObject* calltype = (ct == CallType::_CDECL)
            ? g_zugbruecke_cdecl
            : g_zugbruecke_stdcall;

        if (!calltype)
        {
            throw MiscError("zugbreucke missing support for call type " + std::string
                (
                    (ct == CallType::_CDECL)
                    ? "cdecl (zugbruecke.cdll.LoadLibrary)"
                    : "stdcall (zugbruecke.windll.LoadLibrary)"
                )
            );
        }

        PyObject* py_path = PyUnicode_DecodeFSDefault(path);
        PyObject* py_tuple = PyTuple_New(1);
        PyTuple_SetItem(py_tuple, 0, py_path);

        ed.m_dl = PyObject_CallObject(calltype, py_tuple);

        Py_DECREF(py_path);
        Py_DECREF(py_tuple);

        if (!ed.m_dl)
        {
            throw MiscError("(Zugbruecke) Failed to load library \"" + std::string(path) + "\"");
        }
        g_path_to_dll[path] = ed.m_dl;
        g_dll_refc[ed.m_dl] = 1;
    }
    else
    {
        ed.m_dl = g_path_to_dll[path];
        ++g_dll_refc[ed.m_dl];
    }

    PyObject* dl = static_cast<PyObject*>(ed.m_dl);
    PyObject* fndl = PyObject_GetAttrString(dl, fnname);
    ed.m_dll_fn_address = fndl;

    if (!ed.m_dll_fn_address)
    {
        throw MiscError("(Zugbruecke) failed to find symbol \"" + std::string(fnname) +
            "\" in library \"" + path + "\".");
    }

    PyObject* c_real = PyObject_GetAttrString(g_zugbruecke, "c_double");
    PyObject* c_string = PyObject_GetAttrString(g_zugbruecke, "c_char_p");

    PyObject* restype = (sig_char(rettype) == 'r')
        ? c_real
        : c_string;
    PyObject* argtypes = PyTuple_New(argc);
    for (size_t i = 0; i < argc; ++i)
    {
        PyTuple_SetItem(argtypes, i,
            sig_char(argt[i]) == 'r'
            ? c_real
            : c_string
        );
    }

    // set return and argument types for function.
    PyObject_SetAttrString(fndl, "argtypes", argtypes);
    PyObject_SetAttrString(fndl, "restype", restype);

    Py_DECREF(c_real);
    Py_DECREF(c_string);
    Py_DECREF(argtypes);
    Py_DECREF(restype);

    // reattach sigint interrupt if Python stole it.
    if (staticExecutor.m_debugger) staticExecutor.m_debugger->on_attach();

    external_id_t id = get_next_id();
    g_dlls[id] = ed;
    return id;
}

std::string _str_pyobject(PyObject* o)
{
    PyObject* repr = PyObject_Repr(o);
    PyObject* str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");;
    std::string s = PyBytes_AS_STRING(str);

    Py_XDECREF(repr);
    Py_XDECREF(str);

    return s;
}

void external_call_dispatch_zugbruecke(VO out, std::string sig, void* fn, byte argc, const Variable* argv)
{
    PyObject* _fn = static_cast<PyObject*>(fn);

    // construct args
    PyObject* arguments = PyTuple_New(argc);
    for (size_t i = 0; i < argc; ++i)
    {
        PyObject* arg;
        if (sig[i + 1] == 's')
        {
            arg = PyUnicode_DecodeFSDefault(argv[i].castCoerce<std::string>().c_str());
        }
        else if (sig[i + 1] == 'r')
        {
            arg = PyFloat_FromDouble(argv[i].castCoerce<real_t>());
        }
        else
        {
            throw MiscError("Can only pass strings or reals to external calls.");
        }

        PyTuple_SetItem(arguments, i, arg);
        //Py_DECREF(arg); // seems to cause an error
    }

    if (!PyCallable_Check(_fn))
    {
        throw MiscError("(Zugbruecke) external function not callable. (This is an internal bug.)");
    }

    PyObject* retval = PyObject_CallObject(_fn, arguments);

    if (retval)
    {
        if (sig[0] == 'r')
        // return value is a real
        {
            out = PyFloat_AsDouble(retval);
        }
        else
        {
            PyObject* uf = PyUnicode_AsUTF8String(retval);
            out = std::string(PyBytes_AS_STRING(uf));
            Py_DECREF(uf);
        }
        Py_DECREF(retval);
    }
    else
    {
        // TODO: does void mean 0?
        out = 0;
    }

    Py_DECREF(arguments);

    // reattach sigint interrupt if Python stole it.
    if (staticExecutor.m_debugger) staticExecutor.m_debugger->on_attach();
}
#endif

#ifdef PELOADER
external_id_t external_define_peloader_impl(const char* path, const char* fnname, CallType ct, VariableType rettype, uint32_t argc, VariableType* argt)
{
    ExternalDefinitionDL ed;
    ed.m_ct = ct;
    ed.m_sig.push_back(sig_char(rettype));
    ed.m_dl_path = path;
    ed.m_dl = nullptr;
    for (size_t i = 0; i < argc; ++i)
    {
        ed.m_sig.push_back(sig_char(argt[i]));
    }

    if (g_path_to_dll.find(path) == g_path_to_dll.end())
    {
        if (_c_load_library(path))
        {
            throw MiscError("(PeLoader) Error loading DLL library.");
        }

        g_path_to_dll[path] = ed.m_dl;
        g_dll_refc[ed.m_dl] = 1;

    }
    else
    {
        ed.m_dl = g_path_to_dll[path];
        ++g_dll_refc[ed.m_dl];
    }

    if (_c_get_export(fnname, &ed.m_dll_fn_address) == -1)
    {
        throw MiscError("unable to find symbol \"" + std::string(fnname) + "\".");
    }

    external_id_t id = get_next_id();
    g_dlls[id] = ed;
    return id;
}
#endif

external_id_t external_define_impl(const char* path, const char* fnname, CallType ct, VariableType rettype, uint32_t argc, VariableType* argt)
{
    dlerror();
    ExternalDefinitionDL ed;
    ed.m_ct = ct;
    ed.m_sig.push_back(sig_char(rettype));
    ed.m_dl_path = path;
    for (size_t i = 0; i < argc; ++i)
    {
        ed.m_sig.push_back(sig_char(argt[i]));
    }

    if (g_path_to_dll.find(path) == g_path_to_dll.end())
    {
        ed.m_dl = dlopen(path, RTLD_LAZY);
        g_path_to_dll[path] = ed.m_dl;
        g_dll_refc[ed.m_dl] = 1;
    }
    else
    {
        ed.m_dl = g_path_to_dll[path];
        ++g_dll_refc[ed.m_dl];
    }

    if (ed.m_dl)
    {
        ed.m_dll_fn_address = dlsym(ed.m_dl, fnname);
        if (ed.m_dll_fn_address)
        {
            external_id_t id = get_next_id();
            g_dlls[id] = ed;
            return id;
        }
        else
        {
            throw MiscError("Error loading library \"" + std::string(path) + "\" symbol \"" + fnname + "\": " + dlerror());
        }
    }
    else
    {
        throw MiscError("Error loading library \"" + std::string(path) + "\": " + dlerror());
    }
}

void external_call_impl(VO out, external_id_t id, byte argc,  const Variable* argv)
{
    if (g_dlls.find(id) != g_dlls.end())
    {
        ExternalDefinitionDL& ed = g_dlls.at(id);
        #ifdef EMBED_ZUGBRUECKE
        if (ed.m_zugbruecke)
        {
            external_call_dispatch_zugbruecke(out, ed.m_sig, ed.m_dll_fn_address, argc, argv);
            return;
        }
        #endif
        external_call_dispatch(out, ed.m_sig, ed.m_dll_fn_address, argc, argv);
        return;
    }

    throw MiscError("external call id not recognized.");
}

void external_free_impl(external_id_t id)
{
    auto iter = g_dlls.find(id);
    if (iter != g_dlls.end())
    {
        if (std::get<1>(*iter).m_dl)
        {
            if (--g_dll_refc[std::get<1>(*iter).m_dl] == 0)
            {
                #ifdef EMBED_ZUGBRUECKE
                if (std::get<1>(*iter).m_zugbruecke)
                {
                    PyObject* dl = static_cast<PyObject*>(std::get<1>(*iter).m_dl);
                    Py_DECREF(dl);
                }
                else
                #endif
                {
                    dlclose(std::get<1>(*iter).m_dl);
                }
                g_path_to_dll.erase(std::get<1>(*iter).m_dl_path);
            }
            g_dlls.erase(iter);
        }

        #ifdef EMBED_ZUGBRUECKE
        if (std::get<1>(*iter).m_zugbruecke)
        {
            PyObject* fndl = static_cast<PyObject*>(std::get<1>(*iter).m_dll_fn_address);
            Py_DECREF(fndl);
        }
        #endif
    }
}

#endif

#if defined(_WIN32) || defined(WIN32)
#define RESOLVED

namespace
{
    struct ExternalDefinitionWin32
    {
        HINSTANCE m_dll;
        std::string m_dll_path;
        FARPROC m_dll_fn_address;
        CallType m_ct;
        std::string m_sig;
    };
    std::map<external_id_t, ExternalDefinitionWin32> g_dlls;
    std::map<std::string, HINSTANCE> g_path_to_dll;
    std::map<HINSTANCE, size_t> g_dll_refc;
    bool g_set_dll_directory = false;
}

inline external_id_t get_next_id()
{
    external_id_t i = 0;
    while (g_dlls.find(i) != g_dlls.end())
    {
        ++i;
    }
    ogm_assert(g_dlls.find(i) == g_dlls.end());
    return i;
}

external_id_t external_define_impl(const char* path, const char* fnname, CallType ct, VariableType rettype, uint32_t argc, VariableType* argt)
{
    // dll lookup directory
    if (!g_set_dll_directory)
    {
        if (!SetDllDirectoryA(staticExecutor.m_frame.m_fs.m_included_directory.c_str()))
        {
            std::cout << "Error setting DLL search directory: "
                << GetLastError() << std::endl;
        }
        g_set_dll_directory = true;
    }

    ExternalDefinitionWin32 ed;
    ed.m_ct = ct;
    ed.m_sig.push_back(sig_char(rettype));
    ed.m_dll_path = path;
    for (size_t i = 0; i < argc; ++i)
    {
        ed.m_sig.push_back(sig_char(argt[i]));
    }

    if (g_path_to_dll.find(path) == g_path_to_dll.end())
    {
        ed.m_dll = LoadLibrary(TEXT(path));
        g_path_to_dll[path] = ed.m_dll;
        g_dll_refc[ed.m_dll] = 1;
    }
    else
    {
        ed.m_dll = g_path_to_dll[path];
        ++g_dll_refc[ed.m_dll];
    }

    if (ed.m_dll)
    {
        ed.m_dll_fn_address = GetProcAddress(ed.m_dll, fnname);
        if (ed.m_dll_fn_address)
        {
            external_id_t id = get_next_id();
            g_dlls[id] = ed;
            ogm_assert(g_dlls.find(id) != g_dlls.end());
            return id;
        }
        else
        {
            throw MiscError("Error loading DLL \"" + std::string(path) + "\": dll contains no symbol \"" + fnname + "\"");
        }
    }
    else
    {
        throw MiscError("Error loading DLL \"" + std::string(path) + "\": file not found.");
    }
}

DEFN_external_call(__cdecl, FARPROC)
DEFN_external_call(__stdcall, FARPROC)

void external_call_impl(VO out, external_id_t id, byte argc,  const Variable* argv)
{
    ExternalDefinitionWin32& ed = g_dlls.at(id);
    switch (ed.m_ct)
    {
    case CallType::_STDCALL:
        {
            external_call_dispatch__stdcall(out, ed.m_sig, ed.m_dll_fn_address, argc, argv);
        }
        break;
    case CallType::_CDECL:
        {
            external_call_dispatch__cdecl(out, ed.m_sig, ed.m_dll_fn_address, argc, argv);
        }
        break;
    default:
        throw MiscError("Invalid call type, expected stdcall or cdecl");
    }
}

void external_free_impl(external_id_t id)
{
    auto iter = g_dlls.find(id);
    if (iter != g_dlls.end())
    {
        if (--g_dll_refc[std::get<1>(*iter).m_dll] == 0)
        {
            FreeLibrary(std::get<1>(*iter).m_dll);
            g_path_to_dll.erase(std::get<1>(*iter).m_dll_path);
        }
        g_dlls.erase(iter);
    }
}

#endif

#ifndef RESOLVED
// default implementation.
external_id_t external_define_impl(const char* path, const char* fnname, CallType ct, VariableType rettype, uint32_t argc, VariableType* agrt)
{
    std::cout << "WARNING: external loading not supported on this platform."
    return 0;
}

void external_call_impl(VO out, external_id_t id, byte argc,  const Variable* argv)
{
    out = static_cast<real_t>(0);
}

void external_free_impl(external_id_t id)
{ }

#endif

}

void ogm::interpreter::fn::external_define(VO out, byte argc, const Variable* argv)
{
    if (argc < 5) throw MiscError("external_define requires at least 5 arguments.");

    // marshall arguments
    string_t path = staticExecutor.m_frame.m_fs.resolve_file_path(argv[0].castCoerce<string_t>());
    string_t fnname = argv[1].castCoerce<string_t>();
    CallType ct = static_cast<CallType>(argv[2].castCoerce<size_t>());
    VariableType rt = static_cast<VariableType>(argv[3].castCoerce<size_t>());
    size_t nargs = argv[4].castCoerce<size_t>();
    if (nargs + 5 != argc) throw MiscError("external_define wrong number of arguments");

    VariableType argt[32];
    ogm_assert(nargs < 32);

    // argument types
    for (size_t i = 0; i < nargs; ++i)
    {
        argt[i] =  static_cast<VariableType>(argv[5 + i].castCoerce<size_t>());
    }

    #ifdef __unix__
    // swap .dll out for .so if one is available.
    if (ends_with(path, ".dll"))
    {
        std::string pathnodll = remove_suffix(path, ".dll");
        if (staticExecutor.m_frame.m_fs.file_exists(pathnodll + ".so"))
        {
            path = pathnodll + ".so";
        }
    }
    #endif
    #ifdef EMBED_ZUGBRUECKE
    if (ends_with(path, ".dll"))
    {
        if (zugbruecke_init())
        {
            out = static_cast<real_t>(external_define_zugbruecke_impl(path.c_str(), fnname.c_str(), ct, rt, nargs, argt));
            return;
        }
    }
    #endif
    #ifdef PELOADER
    if (ends_with(path, ".dll"))
    {
        out = static_cast<real_t>(external_define_peloader_impl(path.c_str(), fnname.c_str(), ct, rt, nargs, argt));
        return;
    }
    #endif
    out = static_cast<real_t>(external_define_impl(path.c_str(), fnname.c_str(), ct, rt, nargs, argt));
}

void ogm::interpreter::fn::external_call(VO out, byte argc, const Variable* argv)
{
    if (argc == 0) throw MiscError("external_call requires id argument.");
    external_call_impl(out, argv[0].castCoerce<external_id_t>(), argc - 1, argv + 1);
}

void ogm::interpreter::fn::external_free(VO out, V id)
{
    external_free_impl(id.castCoerce<external_id_t>());
}