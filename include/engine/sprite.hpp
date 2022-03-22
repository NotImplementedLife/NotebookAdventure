/*! \file sprite.hpp */
#pragma once

#include "oam.hpp"
#include "utils.hpp"
#include "camera.hpp"

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
public:
	ObjVisual(u8 frames_count);
		
	u8 get_frames_count() const;
	
	u16 get_frame(int id) const;
		
	void set_frame(int id, u16 obj_tile_index);
	
	u16 get_crt_gfx() const;
	
	void set_crt_gfx(int frame_id);
	
	~ObjVisual();
};

class Sprite
{
public:
	u16 id;
	ObjAttribute* attr;
	u8 anchx, anchy; // sprite anchor (0..255, scaled to hitbox)
	Hitbox hitbox;
	ObjVisual* visual;
	
	sf24 pos_x, pos_y;
	
	s16 get_actual_x() const;
	s16 get_actual_y() const;
public:
	Sprite(ObjSize size, u16 frames_count);
	
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
	
	/*! \brief get sprite visual 
		\return sprite visual
	 */
	ObjVisual* get_visual() const;
	
	void update_visual();
	
	void update_position(Camera* cam);
	
	~Sprite();
};
