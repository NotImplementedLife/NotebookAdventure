#pragma once

#include <gba.h>

#include "vwf.hpp"

class DialogBox
{
	friend class DialogBackground;
	friend class Vwf;
private:	
	const u32 *bg_tile;
	Vwf *vwf;
public:
	u8 left, top, width, height;
	
	DialogBox(u8 _left,u8 _top, u8 _width, u8 _height, Vwf* _vwf);
	
	/*! \brief reserves VRAM space for displaying text
		\param tileaddr address of first render zone tile 
		\param bg_tile_gfx address of the "background" box tile
	 */
	void set_vram(u32 *tile_addr, const u32* bg_tile_gfx);	
	
	/*! \brief clears dialog VRAM zone
	 */
	void clear();
	
	/*! \brief gets available text render space, in tiles
		\param w box width
		\param h box height
	 */
	static int get_vram_tiles_count(u8 w, u8 h);		
};