/*! \file sprite.hpp */
#pragma once

#include "oam.hpp"
#include "utils.hpp"

class Camera;

/*! \def LOAD_GRIT_SPRITE_TILES(res_name, tile_id, palette_start_index)
	\brief macro to copy grit generated binary graphics into OBJ VRAM
	\param res_name resource name (usually the image file name, without extension)
	\param tile_id OBJ tile position 
	\param palette_start_index paltte correction index (useful if loading different sprites with own palettes)
 */
#define LOAD_GRIT_SPRITE_TILES(res_name, tile_id, palette_start_index) {\
	dmaCopy( res_name ## Tiles,(u8*)SPR_VRAM((tile_id)), res_name ## TilesLen); \
	if(palette_start_index!=0) { \
		dmaCopy( ((u8*)(res_name ## Pal)) + 2, (u16*)(SPRITE_PALETTE)+palette_start_index, (res_name ## PalLen)-2); \
		reindex_palette((u16*)SPR_VRAM((tile_id)), res_name ## TilesLen, (palette_start_index)-1);\
	} else  \
		dmaCopy( ((u8*)(res_name ## Pal)), (u16*)(SPRITE_PALETTE), (res_name ## PalLen)); \
	}
	
	

class Hitbox
{
public:
	u8 left, top, width, height;
	
	/*! \brief creates an empty hitbox*/
	Hitbox();
	
	/*! \brief creates a Hitbox instance 
		\param left    hitbox left coordinate
		\param top     hitbox top coordinate
		\param width   hitbox width
		\param height  hitbox height
	 */
	Hitbox(u8 left, u8 top, u8 width, u8 height);
	
	/*! \brief creates hitbox component that fits an object size type
		\param size size of an object that may correspond to the hitbox
	 */
	Hitbox(ObjSize size);
};

class ObjVisual
{
private:
	u16* frames;
	u8 frames_count;	
	u8 crt_gfx_id;
	
	bool anim_loop;
	bool anim_enabled;
	u8 anim_fset;
	u8 anim_id;
	
	u8* framesets;
	
	u8 max_ticks;
	u8 crt_ticks;	
public:
	ObjVisual(u8 frames_count);	
		
	u8 get_frames_count() const;
	
	u16 get_frame(int id) const;
		
	void set_frame(int id, u16 obj_tile_index);
	
	void set_animation_frames(u8 frameset_id, ...);
	
	void set_animation_track(u8 frameset_id, bool loop=true);
	
	void set_ticks(u8 ticks);
	
	u16 get_crt_gfx() const;	
	
	void set_crt_gfx(int frame_id);
	
	void update();
	
	~ObjVisual();
};

const int ANIM_FRAMES_0 = 0;
const int ANIM_FRAMES_1 = 1;
const int ANIM_FRAMES_2 = 2;
const int ANIM_FRAMES_3 = 3;
const int ANIM_FRAMES_4 = 4;
const int ANIM_FRAMES_5 = 5;
const int ANIM_FRAMES_6 = 6;
const int ANIM_FRAMES_7 = 7;

class Sprite
{
public:
	u16 id;	
	u8 anchx, anchy; // sprite anchor (0..255, scaled to hitbox)
	Hitbox hitbox;
	ObjVisual* visual;
	
	sf24 pos_x, pos_y;
	
	s16 get_actual_x() const;
	s16 get_actual_y() const;
public:
	const char* _class;
	ObjAttribute* attr;
	Sprite(ObjSize size, u16 frames_count, const char* objclass=NULL);
	
	/*! \brief sets sprite hitbox
		\param left    hitbox left coordinate
		\param top     hitbox top coordinate
		\param width   hitbox width
		\param height  hitbox height
	 */
	void set_hitbox(u8 left, u8 top, u8 width, u8 height);
	
	/*! \brief get sprite hitbox
		\return hitbox coordinates
	 */
	Hitbox get_hitbox() const;
	
	/*! \brief decides sprite's minimal-size hitbox that covers its non-transparent pixels */
	void auto_detect_hitbox();
	
	void set_anchor(u8 x, u8 y);
	
	void set_x(sf24 px);
	void set_y(sf24 py);
	void set_pos(sf24 px, sf24 py);
	void move(sf24 dx, sf24 dy);
	
	sf24 get_pos_x() const;
	sf24 get_pos_y() const;
	
	/*! \brief get sprite visual 
		\return sprite visual
	 */
	ObjVisual* get_visual() const;
	
	void update_visual();
	
	bool is_of_class(const char *xclass);
	
	void update_position(Camera* cam);
	
	/*! \brief checks if sprite hitboxes overlap
		\return true is sprites are collided
	 */
	bool touches(Sprite* spr);
	
	sf24 get_top_coord() const;
	sf24 get_left_coord() const;
	sf24 get_right_coord() const;
	sf24 get_bottom_coord() const;
	
	
	~Sprite();
};

// Sprite anchor shortcut definitions

/*           Name                X    Y  */
#define ANCHOR_TOP_LEFT           0,   0
#define ANCHOR_TOP              128,   0
#define ANCHOR_TOP_RIGHT        255,   0
#define ANCHOR_LEFT               0, 128
#define ANCHOR_CENTER           128, 128
#define ANCHOR_RIGHT            255, 128
#define ANCHOR_BOTTOM_LEFT        0, 255
#define ANCHOR_BOTTOM           128, 255
#define ANCHOR_BOTTOM_RIGHT     255, 255

// Usage : sprite->set_anchor(ANCHOR_CENTER)


