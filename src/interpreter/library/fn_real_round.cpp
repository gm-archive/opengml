#include "libpre.h"
    #include "fn_math.h"
#include "libpost.h"

#include "ogm/interpreter/Variable.hpp"

#include "ogm/common/error.hpp"
#include <algorithm>

using namespace ogm::interpreter;
using namespace ogm::interpreter::fn;

void ogm::interpreter::fn::round(VO out, V v)
{
    out = static_cast<real_t>(std::floor(v.castCoerce<real_t>() + static_cast<real_t>(.5)));
}

void ogm::interpreter::fn::floor(VO out, V v)
{
  out = static_cast<real_t>(std::floor(v.castCoerce<real_t>()));
}

void ogm::interpreter::fn::frac(VO out, V v)
{
  out = static_cast<real_t>(fmod(v.castCoerce<real_t>(), static_cast<real_t>(1.0)));
}

void ogm::interpreter::fn::abs(VO out, V v)
{
  out = std::abs(v.castCoerce<real_t>());
}

void ogm::interpreter::fn::sign(VO out, V v)
{
  auto _v =  v.castCoerce<real_t>();
  if (_v == 0)
    out = static_cast<real_t>(0);
  else if (_v < 0)
    out = static_cast<real_t>(-1);
  else out = static_cast<real_t>(1);
}

void ogm::interpreter::fn::ceil(VO out, V v)
{
  out = static_cast<real_t>(std::ceil(v.castCoerce<real_t>()));
}

void ogm::interpreter::fn::max(VO out, unsigned char n, const Variable* v)
{
  ogm_assert(n >= 1);
  real_t to_return = v[0].castCoerce<real_t>();
  for (byte i=1; i<n; i++)
  {
    if (v[i].castCoerce<real_t>() >= to_return)
      to_return = v[i].castCoerce<real_t>();
  }
  out = to_return;
}

void ogm::interpreter::fn::mean(VO out, byte n, const Variable* v)
{
  ogm_assert(n >= 1);
  auto sum = 0;
  for (byte i=0; i < n; i++)
  {
    sum += v[i].castCoerce<real_t>();
  }
  out = static_cast<real_t>(sum/(real_t)n);
}

void ogm::interpreter::fn::median(VO out, byte n, const Variable* v)
{
  ogm_assert(n >= 1);
  byte req = n/2;
  for (byte i=0;i<n;i++)
  {
    auto cmp = v[i].castCoerce<real_t>();
    byte found = 0;
    for (byte j=0;j<n;j++)
    {
      if (cmp < v[j].castCoerce<real_t>())
        found ++;
    }
    if (found == req)
      out = cmp;
  }
  ogm_assert(false);
}

void ogm::interpreter::fn::min(VO out, byte n, const Variable* v)
{
  ogm_assert(n >= 1);
  auto to_return = v[0].castCoerce<real_t>();
  for (byte i=1; i<n; i++)
  {
    if (v[i].castCoerce<real_t>() < to_return)
      to_return = v[i].castCoerce<real_t>();
  }
  out = to_return;
}

void ogm::interpreter::fn::clamp(VO out, V val, V min, V max)
{
    if (val < min)
        out.copy(min);
    else if (val >= max)
        out.copy(max);
    else
        out.copy(val);
}

void ogm::interpreter::fn::lerp(VO out, V a, V b, V amt)
{
  if (amt < 0)
    out.copy(a);
  if (amt >= b)
    out.copy(b);

  auto _a = a.castCoerce<real_t>();
  auto _b = b.castCoerce<real_t>();
  auto _amt = amt.castCoerce<real_t>();
  out = _amt * (_b - _a) + _a;
}
