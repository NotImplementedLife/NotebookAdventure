#include "background.hpp"

#include "error.hpp"

#define REG_BG_CNT(i) *((vu16 *)(REG_BASE + 0x08 + 0x02*(i)))
#define BG_ON(i) BIT(8+(i))
#define PTR_BG_HOFS(i) ((u16 *)(REG_BASE + 0x10 + 0x04*(i)))
#define PTR_BG_VOFS(i) ((u16 *)(REG_BASE + 0x12 + 0x04*(i)))

Background::Background(u16 id, u16 char_base,u16 map_base,u16 map_width, u16 map_height)
{	
	m_id=id&3;
	REG_DISPCNT |= BG_ON(id);
	REG_BG_CNT(id)  = BG_256_COLOR | BG_SIZE_0 |  BG_MAP_BASE(map_base) | CHAR_BASE(char_base);
	char_base_address = (u16*)(CHAR_BASE_ADR(char_base));
	map_base_address = (u16*)(MAP_BASE_ADR(map_base));
	set_scroll(0,0);
	width = map_width;
	height = map_height;
	ptr_bg_hofs = PTR_BG_HOFS(id);
	ptr_bg_vofs = PTR_BG_VOFS(id);
	old_x=old_y=0x8000;
	set_priority(m_id);
}

void Background::set_priority(u8 priority)
{
	REG_BG_CNT(m_id) &= ~3;
	REG_BG_CNT(m_id) |= priority&3;
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
	if(palette_displacement != 0)
	{		
		len/=2;		
		for(u32 i=0;i<len;i++)
		{		
			u32 word = char_base_address[i];			
			if(word & 0x000000FF) word += palette_displacement <<  0;				
			if(word & 0x0000FF00) word += palette_displacement <<  8;				
			if(word & 0x00FF0000) word += palette_displacement << 16;				
			if(word & 0xFF000000) word += palette_displacement << 24;
			char_base_address[i] = word;
		}
	}
}

void Background::set_map_stream_source(const void* source)
{
	map_stream_source = (u16*)source;
}

void Background::set_scroll(s16 x,s16 y)
{		
	scroll_x = x; 
	scroll_y = y;
	tile_offset_x = scroll_x >> 3;
	tile_offset_y = scroll_y >> 3;
	ptr_bg_hofs[0] = bg_hofs = (u16)(scroll_x & 0x00FF);
	ptr_bg_vofs[0] = bg_vofs = (u16)(scroll_y & 0x00FF);
}

void Background::init() { }

u16 abs(s16 x) { return x<0?-x:x;}

void Background::build_map()
{		
	u16 screen_base_x = bg_hofs>>3;
	u16 screen_base_y = bg_vofs>>3;	
	
	if(old_x==(s16)0x8000 || (abs(old_x-scroll_x)>=30 || abs(old_y-scroll_y)>=20))
	{
		for(int y=0;y<=20;y++)
		{
			u16* target_line = ((u16*)map_base_address) + (((screen_base_y+y)&31)<<5);
			if(0<=tile_offset_y+y && tile_offset_y+y<height)
			{
				u16* source_line = ((u16*)map_stream_source) + width*(tile_offset_y+y);		
				for(int x=0;x<=30;x++)
				{			
					int src_offset=x+ tile_offset_x;
					if(0<=src_offset&&src_offset<width)
						target_line[(screen_base_x+x)&31] = source_line[src_offset];
					else
						target_line[(screen_base_x+x)&31] = 0;
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
	else 
	{
		if(old_x<scroll_x)
		{						
			for(int y=0;y<=20;y++)
			{
				int d=scroll_x-old_x+8;
				u16* target_line = ((u16*)map_base_address) + (((screen_base_y+y)&31)<<5);				
				if(0<=tile_offset_y+y && tile_offset_y+y<height)
				{
					u16* source_line = ((u16*)map_stream_source) + width*(tile_offset_y+y);												
					for(int x=31;d>0;x--)
					{			
						int src_offset=x+ tile_offset_x;
						if(0<=src_offset&&src_offset<width)
							target_line[(screen_base_x+x)&31] = source_line[src_offset];
						else
							target_line[(screen_base_x+x)&31] = 0;
						d-=8;						
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
		else if(old_x>scroll_x)
		{
			for(int y=0;y<=20;y++)
			{
				int d=old_x-scroll_x;
				u16* target_line = ((u16*)map_base_address) + (((screen_base_y+y)&31)<<5);				
				if(0<=tile_offset_y+y && tile_offset_y+y<height)
				{
					u16* source_line = ((u16*)map_stream_source) + width*(tile_offset_y+y);												
					for(int x=0;d>0;x++)
					{			
						int src_offset=x+ tile_offset_x;
						if(0<=src_offset&&src_offset<width)
							target_line[(screen_base_x+x)&31] = source_line[src_offset];
						else
							target_line[(screen_base_x+x)&31] = 0;
						d-=8;						
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
		
		if(old_y<scroll_y)
		{	
			int d=scroll_y-old_y;
			for(int y=20;d>0;y--)
			{				
				u16* target_line = ((u16*)map_base_address) + (((screen_base_y+y)&31)<<5);				
				if(0<=tile_offset_y+y && tile_offset_y+y<height)
				{
					u16* source_line = ((u16*)map_stream_source) + width*(tile_offset_y+y);												
					for(int x=0;x<=30;x++)
					{			
						int src_offset=x+ tile_offset_x;
						if(0<=src_offset&&src_offset<width)
							target_line[(screen_base_x+x)&31] = source_line[src_offset];
						else
							target_line[(screen_base_x+x)&31] = 0;
						d-=8;						
					}					
				}
				else
				{
					for(int x=0;x<=30;x++)
					{							
						target_line[(screen_base_x+x)&31] = 0;
					}
				}						
				d-=8;
			}	
		}
		else if(old_y>scroll_y)
		{	
			int d=old_y-scroll_y;
			for(int y=0;d>0;y++)
			{				
				u16* target_line = ((u16*)map_base_address) + (((screen_base_y+y)&31)<<5);				
				if(0<=tile_offset_y+y && tile_offset_y+y<height)
				{
					u16* source_line = ((u16*)map_stream_source) + width*(tile_offset_y+y);												
					for(int x=0;x<=30;x++)
					{			
						int src_offset=x+ tile_offset_x;
						if(0<=src_offset&&src_offset<width)
							target_line[(screen_base_x+x)&31] = source_line[src_offset];
						else
							target_line[(screen_base_x+x)&31] = 0;
						d-=8;						
					}					
				}
				else
				{
					for(int x=0;x<=30;x++)
					{							
						target_line[(screen_base_x+x)&31] = 0;
					}
				}						
				d-=8;
			}	
		}
	}
	
	old_x=scroll_x;
	old_y=scroll_y;
}

void Background::render() { }


void Background::key_down(u16 keys) { }

void Background::key_held(u16 keys) { }
	
void Background::key_up(u16 keys) { }

Background::~Background() = default;





