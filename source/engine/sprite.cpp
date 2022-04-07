#include "sprite.hpp"
#include "error.hpp"

#include "camera.hpp"

#include <stdarg.h>
#include <string.h>
#include "debug.hpp"

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
	//dbg_ctx="frames";
	frames = new u16[frames_count];
	//dbg_ctx="framesets";
	framesets = new u8[64]; // 8 animation framesets, max 8 frames each
	
	anim_enabled = false;
	anim_loop = false;
	anim_fset=0;
	
	max_ticks = 1;
	crt_ticks = 0;
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

void ObjVisual::set_ticks(u8 ticks)
{
	max_ticks = ticks;
}

void ObjVisual::set_animation_frames(u8 frameset_id, ...)
{
	va_list args;
	va_start(args,frameset_id);
	u8 k=0;
	for(u8 x; k<8 && ((x = va_arg(args,int)) != 0xFF); k++)
	{
		framesets[(frameset_id<<3)+k]=x;	
	}	
	for(;k<8;k++) 
	{
		framesets[(frameset_id<<3)+k]=0xFF;
	}	
	
	va_end(args);	
}

void ObjVisual::set_animation_track(u8 frameset_id, bool loop)
{
	if(frameset_id>=8)
		fatal(ERR_ARG_OUT_OF_RANGE, "ObjVisual::set_animation_track()");
	
	anim_enabled = true;
	anim_fset=frameset_id;		
	anim_id = (frameset_id<<3);
	anim_loop = loop;
}

u16 ObjVisual::get_crt_gfx() const
{
	if(!anim_enabled)
		return frames[crt_gfx_id];	
	
	return frames[framesets[anim_id]];
}
	
void ObjVisual::set_crt_gfx(int frame_id)
{
	if(frame_id>=frames_count)
		fatal(ERR_ARG_OUT_OF_RANGE, "ObjVisual::set_crt_gfx()");
	anim_enabled = false;
	crt_gfx_id=frame_id;
}

void ObjVisual::update()
{
	if(!anim_enabled) 
		return;
	crt_ticks++;
	if(crt_ticks>=max_ticks)
	{
		crt_ticks=0;
		// next anim frame 
		u8 index = anim_id & 7;
		anim_id &= ~7;		
		
		if(index==7 && !anim_loop)
		{
			anim_enabled=false;
		}
		
		index=(index+1)&7;		
		if(framesets[anim_id | index]!=0xFF) 
			anim_id |= index;
		else if(!anim_loop)
		{
			anim_enabled=false;
		}			
	}
}

ObjVisual::~ObjVisual()
{	
	//dbg_ctx="frames";
	delete[] frames;
	//dbg_ctx="framesets";
	delete[] framesets;
}

Sprite::Sprite(ObjSize size, u16 frames_count, const char* _class)
{		
	id = OamPool::add_obj(ObjAttribute(size,0,0,0));
	attr = OamPool::get_object_by_id(id);
	hitbox = Hitbox(size);	
	//dbg_ctx="ObjVisual";
	visual = new ObjVisual(frames_count);
	this->_class=_class;
	attr->set_priority(1);
}

void Sprite::set_hitbox(u8 left, u8 top, u8 width, u8 height)
{
	hitbox = Hitbox(left, top, width, height);
}

void Sprite::auto_detect_hitbox()
{
	u32* tileaddr = SPR_VRAM(attr->get_tile_index());	
	bool _1D = REG_DISPCNT & OBJ_1D_MAP;
	Hitbox hb(attr->get_size());
	u8 wcnt = hb.width>>3;
	u8 hcnt = hb.height>>3;
	
	u8 x0=hb.width, y0=hb.height, x1=0, y1=0;
	if(_1D)
	{
		//dbg_ctx="_l8";
		u8 *line = new u8[8]; // to do: switch to static array
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
		//dbg_ctx="_l8";
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

void Sprite::set_x(sf24 px) { pos_x = px; }

void Sprite::set_y(sf24 py) { pos_y = py; }

void Sprite::set_pos(sf24 px, sf24 py)
{
	pos_x = px;
	pos_y = py;
}

void Sprite::move(sf24 dx, sf24 dy)
{
	pos_x += dx;
	pos_y += dy;
}

sf24 Sprite::get_pos_x() const
{
	return pos_x;
}

sf24 Sprite::get_pos_y() const
{
	return pos_y;
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
	visual->update();
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
		attr->set_x((s16)pos_x - hitbox.left - anchx - cam->get_x() + 240/2);
		attr->set_y((s16)pos_y - hitbox.top  - anchy - cam->get_y() + 160/2);
	}
}

sf24 Sprite::get_top_coord() const
{
	return pos_y - anchy;
}

sf24 Sprite::get_bottom_coord() const
{
	return pos_y + hitbox.height-anchy;
}

sf24 Sprite::get_left_coord() const
{
	return pos_x - anchx;
}
sf24 Sprite::get_right_coord() const
{
	return pos_x + hitbox.width-anchx;
}

bool Sprite::touches(Sprite* spr)
{
	if(this->attr->is_hidden() || spr->attr->is_hidden())
		return false;
	sf24 x0 = pos_x - anchx;
	sf24 y0 = pos_y -anchy;
	s16 w0 = hitbox.width;
	s16 h0 = hitbox.height;	
	
	
	sf24 x1 = spr->pos_x - spr->anchx;
	sf24 y1 = spr->pos_y - spr->anchy;
	s16 w1 = spr->hitbox.width;
	s16 h1 = spr->hitbox.height;	
	
	return (x0.in_range(x1,x1+w1) || x1.in_range(x0, x0+w0)) 
	    && (y0.in_range(y1,y1+h1) || y1.in_range(y0, y0+h0));
}

bool Sprite::is_of_class(const char *xclass)
{
	return !strcmp(_class, xclass);
}

Sprite::~Sprite()
{
	OamPool::remove_obj(id);	
	//dbg_ctx="ObjVisual";
	delete visual;	
	//((u32*)VRAM)[0]++;
}