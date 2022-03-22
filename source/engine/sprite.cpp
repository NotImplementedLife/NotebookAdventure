#include "sprite.hpp"
#include "error.hpp"

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

ObjVisual::ObjVisual(u8 frames_count)
{
	this->frames_count = frames_count;
	frames = new u16[frames_count];
}

u8 ObjVisual::get_frames_count() const
{
	return frames_count;
}

u16 ObjVisual::get_frame(int id) const
{
	if(id>=frames_count)
		fatal(ERR_ARG_OUT_OF_RANGE, "ObjVisual::get_frame()");
	return frames[id];
}

void ObjVisual::set_frame(int id, u16 obj_tile_index)
{
	if(id>=frames_count)
		fatal(ERR_ARG_OUT_OF_RANGE, "ObjVisual::set_frame()");
	frames[id]=obj_tile_index;
}

u16 ObjVisual::get_crt_gfx() const
{
	return frames[crt_gfx_id];
}
	
void ObjVisual::set_crt_gfx(int frame_id)
{
	if(frame_id>=frames_count)
		fatal(ERR_ARG_OUT_OF_RANGE, "ObjVisual::set_crt_gfx()");
	crt_gfx_id=frame_id;
}

ObjVisual::~ObjVisual()
{	
	delete[] frames;
}

Sprite::Sprite(ObjSize size, u16 frames_count)
{
	id = OamPool::add_obj(ObjAttribute(size,0,0,0));
	attr = OamPool::get_object_by_id(id);
	hitbox = Hitbox(size);
	visual = new ObjVisual(frames_count);	
}

void Sprite::set_hitbox(u8 left, u8 top, u8 width, u8 height)
{
	hitbox = Hitbox(left, top, width, height);
}

void Sprite::auto_detect_hitbox()
{
	u32* tileaddr = SPR_VRAM(attr->get_tile_index());	
	bool _1D = REG_DISPCNT & OBJ_1D_MAP;
	Hitbox hb = Hitbox(attr->get_size());
	u8 wcnt = hb.width>>3;
	u8 hcnt = hb.height>>3;
	
	u8 x0=hitbox.width, y0=hitbox.height, x1=0, y1=0;
	if(_1D)
	{
		u8 *line = new u8[8];
		for(int ty=0;ty<hcnt;ty++)		
			for(int tx=0;tx<wcnt;tx++)
			{				
				for(int y=0;y<8;y++)
				{														
					*((u32*)line + 0) = *(tileaddr++);
					*((u32*)line + 1) = *(tileaddr++);					
					for(int x=0;x<8;x++)					
						if(line[x]) // non-transparent pixel
						{							
							u8 rx = (tx<<3)|x;
							u8 ry = (ty<<3)|y;							
							if(rx < x0) x0=rx;
							if(rx > x1) x1=rx;
							if(ry < y0) y0=ry;
							if(ry > y1) y1=ry;							
						}					
				}				
			}		
		delete[] line;
	}
	else
	{
		fatal("2D sprites are not yet implemented!");
	}		
	if(x0!=hb.width)
	{				
		hitbox = Hitbox(x0, y0, x1-x0+1, y1-y0+1);
	}
	else 
	{		
		hitbox = Hitbox(0,0,0,0);
	}
}

Hitbox Sprite::get_hitbox() const
{
	return hitbox;
}

ObjVisual* Sprite::get_visual() const
{
	return visual;
}

void Sprite::set_anchor(u8 x, u8 y)
{
	anchx = (x * hitbox.width) >> 8;
	anchy = (y * hitbox.height) >> 8;
}

s16 Sprite::get_actual_x() const
{
	return (s16)pos_x - hitbox.left - anchx;
}

s16 Sprite::get_actual_y() const
{
	return (s16)pos_y - hitbox.top - anchy;
}

void Sprite::update_visual()
{
	attr->set_tile_index(visual->get_crt_gfx());
}

void Sprite::update_position(Camera* cam)
{
	if(cam==NULL)
	{
		attr->set_x((s16)pos_x);
		attr->set_y((s16)pos_y);		
	}
	else
	{
		attr->set_x(get_actual_x() - cam->get_x() + 240/2);
		attr->set_y(get_actual_y() - cam->get_y() + 160/2);
	}
}

Sprite::~Sprite()
{
	OamPool::remove_obj(id);
	delete visual;
}