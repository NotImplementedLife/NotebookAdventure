#include "base.hpp"

#ifdef MEM_DBG

#include <cstdlib>

u32 allocs_len = 0, allocs_cap = 0;
u32* allocs_addr = NULL;
u32* allocs_size = NULL;

void redim(u32* &cont,u32 old_len, u32 sup_len)
{
	u32* new_cont = (u32*)malloc((old_len+sup_len)*sizeof(u32));
	
	for(u32 i=0;i<old_len;i++)
	{
		new_cont[i]=cont[i];
	}	
	free(cont);
	cont=new_cont;
}

void add_alloc(void* ptr,u32 sz)
{
	if(allocs_addr==NULL)
	{
		allocs_cap = 512;
		allocs_addr=(u32*)malloc(allocs_cap * sizeof(u32));
		allocs_size=(u32*)malloc(allocs_cap * sizeof(u32));		
	}
	if(allocs_len==allocs_cap)
	{
		redim(allocs_addr, allocs_cap, 256);
		redim(allocs_size, allocs_cap, 256);
		allocs_cap+=256;		
	}
	allocs_addr[allocs_len]=(u32)ptr;
	allocs_size[allocs_len]=sz;
	allocs_len++;
}

u32 rem_alloc(void* ptr)
{
	u32 p=allocs_len;
	u32 val=0;
	for(u32 i=0;i<allocs_len;i++)
	{
		if(allocs_addr[i]==(u32)ptr)
		{
			p=i;
			val=allocs_size[i];
			break;
		}
	}
	for(u32 i=p;i<allocs_len-1;i++)
	{
		allocs_addr[i]=allocs_addr[i+1];
		allocs_size[i]=allocs_size[i+1];
	}
	if(p!=allocs_len)
	{
		allocs_len--;
		return val;
	}
	return 0;
}

u32 crt_allocated_memory = 0;
u32 max_allocated_memory = 0;

void* operator new(size_t size)
{
	crt_allocated_memory += size;
	if(crt_allocated_memory>max_allocated_memory)
	{
		max_allocated_memory = crt_allocated_memory;
	}
	
	void * p = malloc(size);
	
	add_alloc(p,size);	
	
    return p;
}

void operator delete(void* p)
{	
	crt_allocated_memory-=rem_alloc(p);	
	free(p);
}

#endif

const char HEX_DIGIT[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

#ifdef MEM_DBG

u32 get_crt_allocated_memory() 
{
	return crt_allocated_memory;
}

u32 get_max_allocated_memory()
{
	return max_allocated_memory;
}

#else
	
u32 get_crt_allocated_memory() { return 0xFFFFFFFF; }

u32 get_max_allocated_memory() { return 0xFFFFFFFF; }
	
#endif

