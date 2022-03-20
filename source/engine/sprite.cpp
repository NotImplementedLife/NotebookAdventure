#include "sprite.hpp"

Hitbox::Hitbox()
{
	left = top = width = height = 0;
}

Hitbox::Hitbox(u8 left, u8 top, u8 width, u8 height)
{
	this->left   = left;
	this->top    = top;
	this->width  = width;
	this->height = height;
}

static const int objsize_to_width  [] = {  8, 16, 32, 64, 16, 32, 32, 64,  8,  8, 16, 32 };
static const int objsize_to_height [] = {  8, 16, 32, 64,  8,  8, 16, 32, 16, 32, 32, 64 };

Hitbox::Hitbox(ObjSize size)
{
	left = top = 0;
	width  = objsize_to_width[size];
	height = objsize_to_height[size];
}

Sprite::Sprite(ObjSize size)
{
	id = OamPool::add_obj(ObjAttribute(size,0,0,0));
	attr = OamPool::get_object_by_id(id);
	hitbox = Hitbox(size);
}

void Sprite::set_hitbox(u8 left, u8 top, u8 width, u8 height)
{
	hitbox = Hitbox(left, top, width, height);
}

void Sprite::auto_detect_hitbox()
{
	//u16* tileid = attr->get_tile_index();
	
}
