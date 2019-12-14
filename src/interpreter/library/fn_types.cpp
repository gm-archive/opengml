#include "library.h"
#include "ogm/interpreter/Variable.hpp"
#include "ogm/common/error.hpp"
#include "ogm/common/util.hpp"

#include <string>
#include "ogm/common/error.hpp"
#include <locale>
#include <cctype>

using namespace ogm::interpreter;
using namespace ogm::interpreter::fn;

void ogm::interpreter::fn::is_string(VO out, V v)
{
    out = v.get_type() == VT_STRING;
}

void ogm::interpreter::fn::is_array(VO out, V v)
{
    out = v.get_type() == VT_ARRAY;
}

void ogm::interpreter::fn::is_real(VO out, V v)
{
    out = v.get_type() == VT_REAL;
}

void ogm::interpreter::fn::is_bool(VO out, V v)
{
    out = v.get_type() == VT_BOOL;
}


void ogm::interpreter::fn::is_ptr(VO out, V v)
{
    out = v.get_type() == VT_PTR;
}

void ogm::interpreter::fn::is_undefined(VO out, V v)
{
    out = v.get_type() == VT_UNDEFINED;
}

void ogm::interpreter::fn::is_int32(VO out, V v)
{
    out = v.get_type() == VT_INT;
}

void ogm::interpreter::fn::is_int64(VO out, V v)
{
    out = v.get_type() == VT_UINT64;
}

void ogm::interpreter::fn::ogm_typeof(VO out, V v)
{
    switch (v.get_type())
    {
    case VT_REAL:
        out = "number";
        return;
    case VT_STRING:
        out = "string";
        return;
    case VT_ARRAY:
        out = "array";
        return;
    case VT_BOOL:
        out = "bool";
        return;
    case VT_INT:
        out = "int32";
        return;
    case VT_UINT64:
        out = "int64";
        return;
    case VT_PTR:
        out = "ptr";
        return;
    default:
        out = "unknown";
        return;
    }
}

namespace
{
    template<size_t L>
    void is_real_array_l(VO out, V v)
    {
        if (v.get_type() == VT_ARRAY)
        {
            if (v.array_height() == 1)
            {
                if (v.array_length(1) == L)
                {
                    for (size_t i = 0; i < L; ++i)
                    {
                        if (v.array_at(0, i).get_type() != VT_REAL)
                        {
                            out = false;
                            return;
                        }
                    }

                    out = true;
                    return;
                }
            }
        }

        out = false;
    }
}

void ogm::interpreter::fn::is_matrix(VO out, V v)
{
    is_real_array_l<16>(out, v);
}

void ogm::interpreter::fn::is_vec2(VO out, V v)
{
    is_real_array_l<2>(out, v);
}

void ogm::interpreter::fn::is_vec3(VO out, V v)
{
    is_real_array_l<3>(out, v);
}

void ogm::interpreter::fn::is_vec4(VO out, V v)
{
    is_real_array_l<4>(out, v);
}