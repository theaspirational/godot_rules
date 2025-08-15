#ifndef CLIPS_API_6_4_WRAPPER_H
#define CLIPS_API_6_4_WRAPPER_H

extern "C" {
#include "clips_core_6_4/clips.h"
}

// Avoid leaking CLIPS comparator macros into Godot headers (e.g., keyboard.h EQUAL)
#ifdef EQUAL
#undef EQUAL
#endif
#ifdef LESS_THAN
#undef LESS_THAN
#endif
#ifdef GREATER_THAN
#undef GREATER_THAN
#endif

#endif // CLIPS_API_6_4_WRAPPER_H


