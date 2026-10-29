#pragma once

#if defined(ATREUS_PROFILING)
#  include <tracy/Tracy.hpp>
#  define ATREUS_PROFILE_FRAME() FrameMark
#  define ATREUS_PROFILE_SCOPE() ZoneScoped
#  define ATREUS_PROFILE_SCOPE_N(name) ZoneScopedN(name)
#  define ATREUS_PROFILE_PLOT(name, val) TracyPlot(name, val)
#  define ATREUS_PROFILE_MESSAGE(msg) TracyMessageL(msg)
#else
#  define ATREUS_PROFILE_FRAME()
#  define ATREUS_PROFILE_SCOPE()
#  define ATREUS_PROFILE_SCOPE_N(name)
#  define ATREUS_PROFILE_PLOT(name, val)
#  define ATREUS_PROFILE_MESSAGE(msg)
#endif