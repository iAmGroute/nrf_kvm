#pragma once

#include <nrfx/nrfx.h>
#include <nrfx/soc/nrfx_coredep.h>
#include <lib/nrfx/no_deprecated.h>

#include <lib/Names.h>
#include <lib/async.h>

#define nrf_delay_us(us_time) nrfx_coredep_delay_us(us_time)
