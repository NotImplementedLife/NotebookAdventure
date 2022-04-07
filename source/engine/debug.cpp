#include "debug.hpp"

#include "sram.hpp"
#include <gba.h>
#include <string.h>
#include <cstdarg>
#include <cstdlib>

#include "error.hpp"

#ifdef DEBUG_ENABLED

int cnt=0;

const char* dbg_ctx="";

void * operator new(size_t size)
{	
	//void * p = ::operator new(size);
	void * p = malloc(size);
	DEBUG_MSG("%x. + %x : %i  [%s]\n",cnt, p,size,dbg_ctx);
	dbg_ctx="";
	cnt++;	
	return p;
}

void operator delete(void * p)
{	
	DEBUG_MSG("%x. - %x  [%s]\n",cnt, p, dbg_ctx);
	dbg_ctx="";
	cnt++;	
	free(p);
}

#endif

namespace Debug
{
	static u32 dbg_offset = 0x1000;
	
	void init()
	{
		int _0[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		for(int i=0;i<1000;i++)
		{
			SaveTool::write(_0, 64, 0x1000+64*i);
		}		
		dbg_offset = 0x1000;
		write("\nDebug inited\n");
	}
	
	void write(const char* str)
	{
		u32 len = strlen(str);
		SaveTool::write(str, len, dbg_offset);
		dbg_offset+=len;
	}
	
	void write(const char* str, u32 len)
	{		
		SaveTool::write(str, len, dbg_offset);
		dbg_offset+=len;
	}
		
	#ifdef DEBUG_ENABLED
	int DEBUG_FLAG  = ([](){Debug::init(); return 1;})();
	#endif
}

//#include "error.hpp"

const char* _str_hex = "0123456789ABCDEF";

#ifdef DEBUG_ENABLED
void DEBUG_MSG(const char* fmt, ...)
{	
	va_list args;
    va_start(args, fmt);
	while(*fmt)
	{
		int l=0;
		const char* backup_fmt = fmt;
		for(;*fmt!='%' && *fmt!='\0';++fmt, l++);
		if(l>0)
		{
			Debug::write(backup_fmt,l);
		}
		else if(*fmt=='%')
		{
			++fmt;
			switch(*fmt)
			{
				case 's':
				{
					const char* str = va_arg(args, const char*);
					Debug::write(str);
					++fmt;
					break;
				}
				case 'i':
				{					
					u16 dig;
					s32 n = va_arg(args, s32);
					if(n==0)
					{
						Debug::write("0"); ++fmt; break;
					}
					if(n<0) 
					{
						Debug::write("-");					
						n=-n;
					}			
					s32 m =0, nrz=0;
					for(;n>0;n/=10) 
					{				
						if(n%10==0) nrz++;
						m=m*10+n%10;											
					}					
					for(;m>0;m/=10)
					{
						dig='0'+m%10;
						Debug::write((const char*)&dig);
					}
					for(;nrz--;) Debug::write("0");
					++fmt;
					break;
				}
				case 'x':
				{					
					u16 dig;
					u32 n = va_arg(args, u32);
					u32 m = 0;
					for(int i=0;i<8;i++)
					{
						m=(m<<4)|(n&0xF);
						n>>=4;
					}														
					for(int i=0;i<8;i++)
					{
						dig=_str_hex[m&0xF];
						m>>=4;
						Debug::write((const char*)&dig);
					}					
					++fmt;
					break;
				}
			}
		}			
	}
	va_end(args);	
}
#else
	
void DEBUG_MSG(const char* fmt, ...) {	}
	
#endif













