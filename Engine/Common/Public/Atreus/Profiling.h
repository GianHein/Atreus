#pragma once

#if defined(ATREUS_PROFILING)
#  include <tracy/Tracy.hpp>
#  define ATR_PROFILE_FRAME() FrameMark
#  define ATR_PROFILE_SCOPE() ZoneScoped
#  define ATR_PROFILE_SCOPE_N(name) ZoneScopedN(name)
#  define ATR_PROFILE_PLOT(name, val) TracyPlot(name, val)
#  define ATR_PROFILE_MESSAGE(msg) TracyMessageL(msg)
#else
#  define ATR_PROFILE_FRAME()
#  define ATR_PROFILE_SCOPE()
#  define ATR_PROFILE_SCOPE_N(name)
#  define ATR_PROFILE_PLOT(name, val)
#  define ATR_PROFILE_MESSAGE(msg)
#endif