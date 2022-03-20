/*! \file sprite.hpp */
#pragma once

#include "oam.hpp"

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

class Sprite
{
private:
	u16 id;
	ObjAttribute* attr;
	u8 refpx, refpy; // reference positions (0..255 scaled to sprite's hitbox)
	Hitbox hitbox;
		
public:
	Sprite(ObjSize size);
	
	/*! \brief sets sprite hitbox
		\param left    hitbox left coordinate
		\param top     hitbox top coordinate
		\param width   hitbox width
		\param height  hitbox height
	 */
	void set_hitbox(u8 left, u8 top, u8 width, u8 height);
	
	/*! \brief decides sprite's minimal-size hitbox that covers its non-transparent pixels */
	void auto_detect_hitbox();
};
