#pragma once

#define DEBUG_ENABLED

namespace Debug
{
	void init();
	void write(const char* str);
}


#include <cstdlib>

#ifdef DEBUG_ENABLED

#ifdef ALLOC_OVERRIDE

extern const char* dbg_ctx;

void * operator new(size_t size);

void operator delete(void * p);

#endif

#endif

void DEBUG_MSG(const char* fmt, ...);