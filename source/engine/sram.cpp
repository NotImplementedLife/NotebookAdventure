#include <gba.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include "SRAM.hpp"

#define REG_WAITCNT_NV *(u16*)(REG_BASE + 0x0204)
#define BIT_SET(y, flag)	( y |=  (flag) )
#define MEM_ROM		0x08000000	//!< ROM. No write at all (duh)
#define MEM_SRAM	0x0E000000	//!< Static RAM. 8bit write only

namespace SaveTool
{
	/// https://github.com/GValiente/butano/blob/d8061d021804e4a716b92f608f415d65fc4b4e67/butano/hw/include/bn_hw_sram.h

	const char* save_type = "SRAM_V113";		
	const char save_key[17] = "mPfj3OH2gqxQy0b9";
	
	/// https://devkitpro.org/viewtopic.php?t=8913
	__attribute__((section(".iwram"))) void _copy(const uint8_t* source, int size, uint8_t* destination)
	{
		// This code *maybe* should be in WRAM:
		// http://problemkaputt.de/gbatek.htm#gbacartbackupsramfram (Reading and Writing section)

		volatile const uint8_t* source_ptr = source;
		volatile uint8_t* destination_ptr = destination;

		for(int i = 0; i < size; i++)
		{
			destination_ptr[i] = source_ptr[i];
		}
	}

	__attribute__((section(".iwram"))) void write(const void* source, int size, int offset)
	{
		auto source_ptr = reinterpret_cast<const uint8_t*>(source);
		auto destination_ptr = reinterpret_cast<uint8_t*>(MEM_SRAM) + offset;
		_copy(source_ptr, size, destination_ptr);						
	}
		
			
	__attribute__((section(".iwram"))) void read(void* destination, int size, int offset)
	{
		auto source_ptr = reinterpret_cast<const uint8_t*>(MEM_SRAM) + offset;
		auto destination_ptr = reinterpret_cast<uint8_t*>(destination);
		_copy(source_ptr, size, destination_ptr);
	}
	
	bool check_integrity()
	{
		char key[17];
		read(key,16,0);
		key[16]=0;
		
		if(strcmp(key,save_key))	
		{			
			write(save_key,16,0);
			return false;
		}			
		return true;
	}		
	
	/**char* wram_code[0x328-0x2BC];	
	void init()
	{
		memcpy(wram_code,(void*)_copy,0x14);
		memcpy(wram_code+0x14,(void*)_write,0x24);
		memcpy(wram_code+0x38,(void*)_read,0x34);		
	}
	
	void read(void* destination, int size, int offset)
	{	
		((void (*)(void*, int, int))(wram_code+0x38))(destination,size,offset);
	}
	
	void write(const void* destination, int size, int offset)
	{	
		((void (*)(const void*, int, int))(wram_code+0x14))(destination,size,offset);
	}	*/
}