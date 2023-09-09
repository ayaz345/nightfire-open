#pragma once

#include "engine_builddefs.h"
#include "backends.h"

// Convenience definitions for sanity checks

#if !XASH_DEDICATED() && defined(XASH_SDL)
#define XASH_IS_SDL_CLIENT() 1
#else
#define XASH_IS_SDL_CLIENT() 0
#endif
