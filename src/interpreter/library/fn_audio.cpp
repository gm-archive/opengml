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

using namespace ogm::interpreter;
using namespace ogm::interpreter::fn;

#define frame staticExecutor.m_frame

void ogm::interpreter::fn::audio_is_playing(VO out, V audio)
{
    // TODO
    out = false;
}

void ogm::interpreter::fn::audio_play_sound(VO out, V audio)
{
    // TODO
    out = false;
}


void ogm::interpreter::fn::audio_stop_sound(VO out, V audio)
{
    // TODO
    out = false;
}


void ogm::interpreter::fn::audio_set_gain(VO out, V audio)
{
    // TODO
    out = false;
}