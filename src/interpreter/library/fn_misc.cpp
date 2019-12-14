#include "library.h"
#include "ogm/interpreter/Variable.hpp"
#include "ogm/common/error.hpp"
#include "ogm/common/util.hpp"
#include "ogm/interpreter/Executor.hpp"
#include "ogm/interpreter/execute.hpp"
#include "ogm/interpreter/display/Display.hpp"

#include <string>
#include "ogm/common/error.hpp"
#include <locale>
#include <cctype>
#include <cstdlib>
#include <chrono>

using namespace ogm::interpreter;
using namespace ogm::interpreter::fn;

#define frame staticExecutor.m_frame

void ogm::interpreter::fn::parameter_count(VO out)
{
    out = static_cast<real_t>(frame.m_data.m_clargs.size() - 1);
}

void ogm::interpreter::fn::parameter_string(VO out, V i)
{
    size_t _i = i.castCoerce<size_t>();
    if (_i < frame.m_data.m_clargs.size())
    {
        out = frame.m_data.m_clargs[_i];
    }
}

namespace
{
    // https://stackoverflow.com/a/41582957
    uint64_t get_time()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    // time program starts.
    const uint64_t g_start_time = get_time();
}

void ogm::interpreter::fn::get_timer(VO out)
{
    out = static_cast<real_t>(get_time() - g_start_time);
}

void ogm::interpreter::fn::getv::fps_real(VO out)
{
    // TODO
    out = static_cast<real_t>(60);
}

void ogm::interpreter::fn::getv::fps(VO out)
{
    // TODO
    fps_real(out);
    out = static_cast<real_t>(out.castCoerce<int32_t>());
}


void ogm::interpreter::fn::md5_string_utf8(VO out, V s)
{
    // TODO
    out = "00000000000000000000000000000000";
}

void ogm::interpreter::fn::md5_string_unicode(VO out, V s)
{
    // TODO
    out = "00000000000000000000000000000000";
}