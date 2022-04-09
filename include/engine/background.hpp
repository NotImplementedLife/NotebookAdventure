/*! \file background.hpp	
 */
 
#pragma once

#include <gba.h>

class Background
{
protected:		
	u8 m_id;
	u16 *char_base_address, *map_base_address;
	u16 width, height;
	u16 *map_stream_source;	
	s16 scroll_x, scroll_y;
	s16 tile_offset_x, tile_offset_y;
	u16 *ptr_bg_hofs, *ptr_bg_vofs;
	u16 bg_hofs, bg_vofs;
	s16 old_x=0x8000, old_y=0x8000;

	u32 width_mul_table[256];
	
private:	
	u32 width_mul(u32 factor);
protected:
	/*! \brief loads tileset graphics into its specific VRAM block
		\param source     tiles buffer data
		\param len        buffer length
		\param compressed specifies if tiles are LZ77 compressed
	 */
	void load_tiles(const void* source,u32 len,bool compressed=false, u8 palette_displacement=0);
	
	/*! \brief sets pointer to the map stream source
		\param map  stream source buffer	
	 */
	void set_map_stream_source(const void* source);
	
	/*! \brief sets background scroll offsets
		\param x background scroll x
		\param y background scroll y
	 */
public:	
	/*! \brief Creates a new Background component.
	    \param id         background id (0..3)
		\param char_base  VRAM char block base (0..3)
		\param map_base   VRAM screen block base (0..31)
		\param map_width  tilemap width in tiles
		\param map_height tilemap height in tiles
	 */
	Background(u16 id, u16 char_base,u16 map_base,u16 map_width, u16 map_height);
	
	void set_priority(u8 priority);
	
	virtual void init();
		
	void set_scroll(s16 x,s16 y);
	
	/*! \brief renders the visible map portion in VRAM
	 */
	virtual void build_map();
	
	virtual void render();
	
	virtual void key_down(u16 keys);
	
	virtual void key_held(u16 keys);
	
	virtual void key_up(u16 keys);
	
	virtual ~Background();
};