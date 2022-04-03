#pragma once
#include <gba.h>

#define MEM_DBG

#ifdef MEM_DBG

#include <cstddef>

extern u32 crt_allocated_memory;
extern u32 max_allocated_memory;

void* operator new(size_t size);
void operator delete(void*);

#endif

u32 get_crt_allocated_memory();
u32 get_max_allocated_memory();

extern const char HEX_DIGIT[16];