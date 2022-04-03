#include "dialogbox.hpp"

	
DialogBox::DialogBox(u8 _left, u8 _top, u8 _width, u8 _height, Vwf* _vwf) : vwf(_vwf),
                                                                            left(_left), top(_top), width(_width), height(_height)																				
{
	
}

int DialogBox::get_vram_tiles_count(u8 w, u8 h)
{	
	return (w-2)*(h-2) + ((w&h)&1);
}
	
void DialogBox::set_vram(u32 *tile_addr, const u32* bg_tile_gfx)
{				
	bg_tile = bg_tile_gfx;
	vwf->set_render_space(tile_addr,width-2,height-2);
	for(int count = get_vram_tiles_count(width,height);count--;tile_addr+=16)
	{
		dmaCopy(bg_tile_gfx, tile_addr, 64);
	}			
}

void DialogBox::clear()
{
	u32* tile_addr = vwf->render_space;
	for(int count = get_vram_tiles_count(width,height);count--;tile_addr+=16)
	{
		dmaCopy(bg_tile, tile_addr, 64);		
	}	
	vwf->reset();
}