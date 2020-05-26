#pragma once

#ifdef __ARM_ARCH_6M__
#include "Instructions/Instructions_armv6m.h"
#else
#include "Instructions/Instructions_soft.h"
#info    "Using Instructions_soft.h"
#endif
