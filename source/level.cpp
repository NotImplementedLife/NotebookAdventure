#include "level.hpp"


#include "notebook-sheet.h"
#include "test_level.h"
#include "test_level_bin.h"
#include "player.h"
#include "dialog_frame.h"
#include "dialog_arr.h"
#include "spikes.h"
#include "burned_hole.h"

#include <string.h>

class LevelBackgroundBage : public Background
{
public:
	LevelBackgroundBage() : Background(3,2,31,75,105) { strcpy(magic,"LevelBgPage"); }
	
	void init() override
	{
		Background::load_tiles(notebook_sheetTiles,notebook_sheetTilesLen, true, 0);
		Background::set_map_stream_source(notebook_sheetMap);
		dmaCopy((u8*)notebook_sheetPal,(u8*)(BG_PALETTE),notebook_sheetPalLen);
	}	
};

class LevelDungeon : public Background
{
public:
	LevelDungeon() : Background(2,0,29,75,105) { strcpy(magic,"LevelDungeon"); }
	
	void init() override
	{		
		Background::load_tiles(test_levelTiles,test_levelTilesLen, true, 160);
		Background::set_map_stream_source(test_levelMap);			
		dmaCopy((u8*)test_levelPal,(u16*)(BG_PALETTE)+160, test_levelPalLen);
	}	
};

static const sf24 g = sf24(2,1);

class PhysicalObject : public Sprite
{	
public:
	sf24 ax, ay;
	sf24 vx, vy;
	sf24 fx, fy;	
	sf24 air_ymin, air_ymax;
	bool use_air_limits=false;
public:
	PhysicalObject(ObjSize size, u16 frames_count) : Sprite(size, frames_count) 
	{
		vx = 0;
		vy = 0;
		
		fx = sf24(2,0);
		fy = sf24(3,0);
		
		air_ymin=0;
		air_ymax=840;
	}
	
	void set_air_limits(s16 ymin, s16 ymax)
	{
		use_air_limits=true;
		air_ymin = ymin;
		air_ymax = ymax;
	}
	
	void ignore_air_limits()
	{
		use_air_limits=false;
	}
	
	void render()
	{
		vx+=ax;
		vy+=ay;
		pos_x+=vx;
		pos_y+=vy;
		if(use_air_limits)
		{
			if(get_top_coord()<air_ymin) 
			{
				pos_y = air_ymin + anchy;
				if(ay<0) ay+=sf24(0,8);
			}
			else if(get_bottom_coord()>=air_ymax)
			{
				pos_y = air_ymax - hitbox.height + anchy + 1;		
				ay=0;
			}
		}
		
		if(ax>0) ax-=sf24(0, 8);
		else if(ax<0) ax+=sf24(0, 8);
		
		if(ay>0) ay-=sf24(0, 8);
		else if(ay<0) ay+=sf24(0,8);
		
		if(vx>0)
		{
			vx-=fx;
			if(vx<0) vx=0;
		}
		else if(vx<0)
		{
			vx+=fx;
			if(vx>0) vx=0;
		}
		
		if(vy>0)
		{
			vy-=fy;
			if(vy<0) vy=0;
		}		
		else if(vy<0)
		{
			vy+=fy;
			if(vy>0) vy=0;
		}
	}
	
	void charge_v(sf24 vx, sf24 vy)
	{
		this->vx+=vx;
		this->vy+=vy;
	}
	
	void charge_a(sf24 ax, sf24 ay)
	{
		this->ax=ax;
		this->ay=ay;
	}
		
	
	void apply_g()
	{
		this->vy+=g;
	}		
};

class Player : public PhysicalObject
{
private:

public:
	Player() : PhysicalObject(SIZE_32x32, 16)
	{
		LOAD_GRIT_SPRITE_TILES(player, 8, 0);
			
		get_visual()->set_frame(0,0x0008);
		get_visual()->set_frame(1,0x0028);
		get_visual()->set_frame(2,0x0048);
		get_visual()->set_frame(3,0x0068);
		
		get_visual()->set_ticks(16);
		
		get_visual()->set_crt_gfx(0);	
		get_visual()->set_animation_frames(ANIM_FRAMES_0, 0,1,2,3, -1);
		get_visual()->set_animation_track(ANIM_FRAMES_0);
		update_visual();
		//set_hitbox(10,0,14,30);
		set_hitbox(14,0,4,30);
		set_anchor(ANCHOR_BOTTOM);		
	}
};

class Spikes : public Sprite
{
public:
	Spikes(ObjSize size) : Sprite(size, 1, "spikes")
	{
		get_visual()->set_frame(0,0x0280);
		get_visual()->set_crt_gfx(0);
		update_visual();
		auto_detect_hitbox();
		set_anchor(ANCHOR_BOTTOM_LEFT);
	}
};

class Hole : public Sprite
{
public:
	Hole(): Sprite(SIZE_32x32,1,"hole")
	{
		get_visual()->set_frame(0,0x0288);
		get_visual()->set_crt_gfx(0);
		set_hitbox(2,2,28,28);
		update_visual();
		set_anchor(ANCHOR_CENTER);
	}
};

class LevelDialog : public DialogBackground
{
private:
	Vwf *vwf;
public:
	LevelDialog() : DialogBackground(0, 1, 28) { strcpy(magic,"LvlDialog"); }
	
	void init() override
	{
		load_tiles(dialog_frameTiles);
		clear_map();
		vwf = new Vwf(defaultFont816);
		create_dialog_box(1, 1, 16, 4, vwf);
		
		dmaCopy(dialog_arrTiles, (u8*)(VRAM + 0x10000 + 0x64*64), 64);
		set_caret(0xC8);		
		
		vwf->set_text_color(0xA2);		
		launch_dialog(0,"Test title 1",200);
	}	
};

//////////////////////////////////////////////////////////////////////////////////////////

Level::Level(const u8* lvl_src) : TextScrollMap()
{
	set_blocks_data(lvl_src);
	cat = NULL;
}

void Level::init() 
{			
	LOAD_GRIT_SPRITE_TILES(spikes, 0x280, 48);
	LOAD_GRIT_SPRITE_TILES(burned_hole, 0x288, 64);

	LevelBackgroundBage* bg_page = new LevelBackgroundBage();
	set_background(3, bg_page, 0x10);
	
	LevelDungeon* dungeon = new LevelDungeon();
	set_background(2, dungeon, 0x10);
	
	dialog = new LevelDialog();
	set_background(0, dialog, 0x00);
	
	REG_BLDCNT = (1<<2) | (1<<6) | (1<<11);
	REG_BLDALPHA = 13 | (3<<8);
	
	explorer = new Sprite(SIZE_8x8,1);
	register_sprite(explorer);
	
	player = new Player();	
	
		
	u8* lvldat = (u8*)blocks_data+75*105;
	
	
	u16 px=0, py=0;
	px = (*(lvldat++));
	px |= (*(lvldat++))<<8;
	
	py = (*(lvldat++));
	py |= (*(lvldat++))<<8;
	
	player->set_pos(px, py);		
	register_sprite(player);
		
	hole = new Hole();	
	
	u16 fx=0, fy=0;	
	fx = (*(lvldat++));
	fx |= (*(lvldat++))<<8;
	
	fy = (*(lvldat++));
	fy |= (*(lvldat++))<<8;
	
	hole->set_pos(fx,fy);
	
	register_sprite(hole);
	
	bool has_cat = (*(lvldat++)!=0);
	
	if(has_cat)
	{
		u16 cx=0, cy=0;	
		cx = (*(lvldat++));
		cx |= (*(lvldat++))<<8;
		
		cy = (*(lvldat++));
		cy |= (*(lvldat++))<<8;
	}
		
	u8 spkcnt = *(lvldat++);
	for(int i=0;i<spkcnt;i++)
	{
		u8 spkx = *(lvldat++);
		u8 spky = *(lvldat++);
		u8 spkl = *(lvldat++);
		
		add_spikes(spkx<<3, (spky+1)<<3, spkl);
	}
	
	set_focus(player);
}

void Level::on_frame() 
{
	u8 bdata = get_block_data((int)focus->get_pos_x(), (int)focus->get_pos_y());	
	if(bdata==0)
	{		
		u8 y0=((int)focus->get_pos_y())>>3;
		u8 ymin=y0, ymax=y0;
		u8 x0=((int)focus->get_pos_x())>>3;
				
		for(int y=y0;y<104;y++)
			if(blocks_data[y*75+x0])
			{
				ymax=y;
				break;
			}			
		for(int y=y0;y>0;y--)
			if(blocks_data[y*75+x0])
			{
				ymin=y;
				break;
			}							
		player->set_air_limits((ymin+1)<<3,ymax<<3);							
		player->apply_g();
	}
	else
	{		
		u8 udata = get_block_data((int)player->get_pos_x(), (int)player->get_pos_y()-1);		
		player->ignore_air_limits();		
		if(bdata==1 || (bdata==0 && udata==2))
		{
			// 8 multiple correction
			player->pos_y = ((int)player->pos_y)&(~7);
		}
		if(bdata==2)
		{
			player->ay=0;
		}		
	}
	
	player->render();
	
	for(int i=0;i<sprites_count;i++)
	{
		if(sprites[i]->is_of_class("spikes"))
		{
			if(player->touches(sprites[i]))
			{				
				dialog->launch_dialog(0,"dead",10);
			}
			else if(player->touches(hole))
			{
				dialog->launch_dialog(0,"good",10);
			}
		}
	}
	
	//dialog->launch_dialog(0,player->pos_y.to_string(),10);
}

void Level::on_key_down(int keys)
{
	if(keys & KEY_UP) 
	{
		s16 px=(int)player->get_pos_x();
		s16 py=(int)player->get_pos_y();
		u8 ddata = get_block_data(px, py+8);
		u8 bdata = get_block_data(px, py);
		u8 udata = get_block_data(px, py-8);
		if(bdata!=0 && udata==0)			
			player->charge_a(0,-sf24(3,160));
	}
	
	if(keys & KEY_B)
	{
		player->ignore_air_limits();
		player->set_pos(100,100);
	}
}

void Level::on_key_held(int keys)
{		
	if(keys & KEY_DOWN)  
	{	
		int block = get_block_data((int)(player->pos_x),(int)focus->get_pos_y());
		if(block==2)
		{				
			focus->move(0,1);
		}
	}
	else if(keys & KEY_UP)  
	{
		int upper = get_block_data((int)(player->pos_x),(int)focus->get_pos_y()-1);
		int above = get_block_data((int)(player->pos_x),(int)focus->get_pos_y()-64);		
		if(upper==2)
		{			
			focus->move(0,-1);
		}
	}
	
	
	if(keys & KEY_LEFT) 
	{
		focus->attr->set_flip_h(0);		
		
		int on_left = get_block_data((int)(player->get_left_coord())-1,(int)focus->get_pos_y()-2);
		on_left |= get_block_data((int)(player->get_left_coord())-1,(int)focus->get_pos_y()-10);
		on_left |= get_block_data((int)(player->get_left_coord())-1,(int)focus->get_pos_y()-18);
		on_left |= get_block_data((int)(player->get_left_coord())-1,(int)focus->get_pos_y()-26);
		
		if(on_left!=1)
			player->charge_v(-sf24(2),0);
		//dialog->launch_dialog(0,player->get_top_coord().to_string(),10);
	}
	else if(keys & KEY_RIGHT) 
	{
		focus->attr->set_flip_h(1);
		
		int on_right = get_block_data((int)(player->get_right_coord())+1,(int)focus->get_pos_y()-2);
		on_right |= get_block_data((int)(player->get_right_coord())+1,(int)focus->get_pos_y()-10);
		on_right |= get_block_data((int)(player->get_right_coord())+1,(int)focus->get_pos_y()-18);
		on_right |= get_block_data((int)(player->get_right_coord())+1,(int)focus->get_pos_y()-26);
		
		if(on_right!=1)
			player->charge_v(sf24(2),0);
				
		//dialog->launch_dialog(0,focus->get_pos_x().to_string(),10);
		//dialog->launch_dialog(0,sf24(get_block_data((int)focus->get_pos_x(), (int)focus->get_pos_y())).to_string(),10);
	}
		
}

void Level::set_blocks_data(const u8* src)
{
	blocks_data=src;
}

u8 Level::get_block_data(s16 x, s16 y) const
{
	return blocks_data[(y>>3)*75+(x>>3)];
}

void Level::set_focus(Sprite* spr)
{	
	focus=spr;	
	camera->follow(focus);	
	focus->update_position(camera);
}


void Level::add_spikes(s16 x, s16 y, s8 len)
{
	while(len>=4)
	{
		Spikes* spikes = new Spikes(SIZE_32x8);
		spikes->set_pos(x,y);
		x+=32;
		len-=4;
		register_sprite(spikes);
	}
	while(len>=2)
	{
		Spikes* spikes = new Spikes(SIZE_16x8);
		spikes->set_pos(x,y);
		x+=16;
		len-=2;
		register_sprite(spikes);
	}	
	while(len>=1)
	{
		Spikes* spikes = new Spikes(SIZE_8x8);
		spikes->set_pos(x,y);
		x+=8;
		len-=1;
		register_sprite(spikes);
	}
	
	
}
