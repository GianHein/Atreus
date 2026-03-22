#pragma once

#include "Atr/Definitions.h"
#include "Atr/Platform/Types/DateTime.h"
#include "Atr/Platform/Types/Duration.h"
#include "Atr/Platform/Types/Timestamp.h"

namespace Atr::Platform
{
  Timestamp GetNow();

  Duration FromSeconds(f64 Seconds);
  Duration FromMilliseconds(f64 Milliseconds);

  DateTime GetUtcTime();
  DateTime GetLocalTime();
}