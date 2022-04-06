#pragma once

#define DEBUG_ENABLED

namespace Debug
{
	void init();
	void write(const char* str);
}


#include <cstdlib>

extern const char* dbg_ctx;

void * operator new(size_t size);

void operator delete(void * p);

void DEBUG_MSG(const char* fmt, ...);