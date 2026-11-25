/**
 * @file    steami_release.h
 * @brief   STeaMi DAPLink release number and date, reported by the vendor
 *          command ID_DAP_SteamiVersion (0x9F).
 *
 * A local build reports release 0, date 0 ("development build").
 * generate_release.yml writes steami_release_ci.h, which is not tracked by
 * git, before compiling: a release build reports the GitHub run number N and
 * the date of its release name YYYY-MM-DD.N. Rewriting this tracked file
 * instead would make DETAILS.TXT report "Local Mods: 1".
 */

#pragma once

#if defined(__has_include)
#if __has_include("steami_release_ci.h")
#include "steami_release_ci.h"
#endif
#endif

/** Release number: the N of release YYYY-MM-DD.N, 0 for a local build. */
#ifndef STEAMI_RELEASE_NUMBER
#define STEAMI_RELEASE_NUMBER 0u
#endif

/** Release date as the decimal number YYYYMMDD, 0 for a local build. */
#ifndef STEAMI_RELEASE_DATE
#define STEAMI_RELEASE_DATE 0u
#endif
