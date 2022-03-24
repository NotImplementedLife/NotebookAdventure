#include "background.hpp"

#include "error.hpp"

#define REG_BG_CNT(i) *((vu16 *)(REG_BASE + 0x08 + 0x02*(i)))
#define BG_ON(i) BIT(8+(i))
#define PTR_BG_HOFS(i) ((u16 *)(REG_BASE + 0x10 + 0x04*(i)))
#define PTR_BG_VOFS(i) ((u16 *)(REG_BASE + 0x12 + 0x04*(i)))

Background::Background(u16 id, u16 char_base,u16 map_base,u16 map_width, u16 map_height)
{	
	REG_DISPCNT |= BG_ON(id);
	REG_BG_CNT(id)  = BG_256_COLOR | BG_SIZE_0 |  BG_MAP_BASE(map_base) | CHAR_BASE(char_base);
	char_base_address = (u16*)(CHAR_BASE_ADR(char_base));
	map_base_address = (u16*)(MAP_BASE_ADR(map_base));
	set_scroll(0,0);
	width = map_width;
	height = map_height;
	ptr_bg_hofs = PTR_BG_HOFS(id);
	ptr_bg_vofs = PTR_BG_VOFS(id);
}

void Background::load_tiles(const void* source,u32 len,bool compressed, u8 palette_displacement)
{
	if(!compressed)
		dmaCopy(source,char_base_address,len);
	else
	{
		LZ77UnCompVram(source, char_base_address);		
		// LZ77 header: 0xSSSSSS10 (https://problemkaputt.de/gbatek.htm#lzdecompressionfunctions)
		//                ^=size 
		len = (*((u32*)source))>>8;
	}
	if(palette_displacement>0)
	{		
		len/=2;
		u32 mask = palette_displacement * 0x01010101;
		u32* cba = (u32*)char_base_address;
		for(u32 i=0;i<len;i++)
		{		
			char_base_address[i] += mask;
		}
	}
}

void Background::set_map_stream_source(const void* source)
{
	map_stream_source = (u16*)source;
}

void Background::set_scroll(u16 x,u16 y)
{		
	scroll_x = x; 
	scroll_y = y;
	tile_offset_x = scroll_x >> 3;
	tile_offset_y = scroll_y >> 3;
	ptr_bg_hofs[0] = bg_hofs = (u16)(scroll_x & 0x00FF);
	ptr_bg_vofs[0] = bg_vofs = (u16)(scroll_y & 0x00FF);
}

void Background::init() { }


void Background::build_map()
{		
	u16 screen_base_x = bg_hofs>>3;
	u16 screen_base_y = bg_vofs>>3;	
	
	for(int y=0;y<=20;y++)
	{
		u16* target_line = ((u16*)map_base_address) + (((screen_base_y+y)&31)<<5);
		if(0<=tile_offset_y+y && tile_offset_y+y<=height+1)
		{
			u16* source_line = ((u16*)map_stream_source) + width*(tile_offset_y+y);		
			for(int x=0;x<=30;x++)
			{			
				int src_offset=x+ tile_offset_x;
				
				target_line[(screen_base_x+x)&31] = source_line[src_offset];
			}
		}
		else
		{
			for(int x=0;x<=30;x++)
			{							
				target_line[(screen_base_x+x)&31] = 0;
			}
		}					
	}
}

void Background::render() { }


void Background::key_down(u16 keys) { }

void Background::key_held(u16 keys) { }
	
void Background::key_up(u16 keys) { }

Background::~Background() = default;





