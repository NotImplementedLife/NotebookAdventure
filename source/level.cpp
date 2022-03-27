#include "level.hpp"


#include "notebook-sheet.h"
#include "test_level.h"
#include "test_level_bin.h"
#include "player.h"
#include "dialog_frame.h"
#include "dialog_arr.h"
#include "spikes.h"
#include "burned_hole.h"
#include "trampoline.h"

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
		
		ax = 0;
		ay = 0;
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

class Trampoline : public Sprite
{
public:
	Trampoline() : Sprite(SIZE_16x16, 2, "tram")
	{
		get_visual()->set_frame(0,0x02A8);
		get_visual()->set_frame(1,0x02B0);
		get_visual()->set_crt_gfx(0);
		update_visual();
		auto_detect_hitbox();
		set_anchor(ANCHOR_BOTTOM);		
		
		get_visual()->set_ticks(25);
		
		get_visual()->set_animation_frames(ANIM_FRAMES_0, 0, 1, 0, -1);		
	}
};

class LevelDialog : public DialogBackground
{
private:
	Vwf *vwf;
	Vwf *vwf_j;
	Vwf *vwf_g;
public:
	LevelDialog() : DialogBackground(0, 1, 28) { strcpy(magic,"LvlDialog"); }
	
	void init() override
	{		
		clear_map();
		load_tiles(dialog_frameTiles);		
		dmaCopy(dialog_arrTiles, (u8*)(VRAM + 0x10000 + 0x64*64), 64);						
		set_caret(0xC8);
		
		// Dialog 0: use to display level title
		vwf = new Vwf(defaultFont816);
		vwf->set_text_color(0xA2);		
		create_dialog_box(1, 1, 16, 4, vwf);		
		
		// Dialog 1: use dialog timings to control jumping on sprites
		// uses:
		// - trampoline (jump on short sprites)
		// - floating object simulation (jump on tall sprites)
		vwf_j = new Vwf(defaultFont816);
		create_dialog_box(0, 22, 3, 4, vwf_j);
		
		// Dialog 2: general purpose user-involving dialog
		// used to display the "game over" message
		
		vwf_g = new Vwf(defaultFont816);
		create_dialog_box(1,14,28,6, vwf_g);		
	}	
};

//////////////////////////////////////////////////////////////////////////////////////////

Level::Level(const u8* lvl_src) : TextScrollMap()
{
	set_blocks_data(lvl_src);
	cat = NULL;	
	completed=false;
}

void Level::init() 
{			
	LOAD_GRIT_SPRITE_TILES(spikes, 0x280, 48);
	LOAD_GRIT_SPRITE_TILES(burned_hole, 0x288, 64);
	LOAD_GRIT_SPRITE_TILES(trampoline, 0x2A8, 56);

	LevelBackgroundBage* bg_page = new LevelBackgroundBage();
	set_background(3, bg_page, 0x10);
	
	LevelDungeon* dungeon = new LevelDungeon();
	set_background(2, dungeon, 0x10);
	
	dialog = new LevelDialog();
	set_background(0, dialog, 0x00);	
	
	dialog->run_on_dialog_finished(1, dialog_controlled_jump, this);		
	
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
	
	
	u8 trpcnt = *(lvldat++);
	for(int i=0;i<trpcnt;i++)
	{
		u8 trpx = *(lvldat++);
		u8 trpy = *(lvldat++);
		
		add_trampoline(trpx<<2, (trpy+1)<<3);
	}
	
	set_focus(player);
}

void Level::on_frame() 
{	
	if(completed)
	{
		this->exit(completed);
		return;
	}
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
				if(!input_locked())
				{
					lock_input(0);
					dialog->run_on_dialog_finished(2, game_over_dialog_finished, this);
					dialog->launch_dialog(2, "Game over!\n                 \x01Retry            \01Menu", 0);				
				}
			}			
						
		}
		else if(sprites[i]->is_of_class("tram"))
		{
			if(player->touches(sprites[i]))
			{	
				sprites[i]->get_visual()->set_animation_track(ANIM_FRAMES_0,false);
				dialog->launch_dialog(1,"i",10);
			}	
		}
		
		else if(player->touches(hole))
		{
			if(!input_locked())
			{
				lock_input(0);
				dialog->run_on_dialog_finished(2, level_completed_dialog_finished, this);
				dialog->launch_dialog(2, "Well played!\n                  \x01Next            \01Menu", 0);				
			}
		}
	}	
}

void Level::on_key_down(int keys)
{
	if(keys & KEY_UP) 
	{
		s16 px=(int)player->get_pos_x();
		s16 py=(int)player->get_pos_y();
		//u8 ddata = get_block_data(px, py+8);
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
		//int above = get_block_data((int)(player->pos_x),(int)focus->get_pos_y()-64);		
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

void Level::add_trampoline(s16 x, s16 y)
{
	Trampoline* tr = new Trampoline();
	tr->set_pos(x,y);
	register_sprite(tr);
}

int Level::dialog_controlled_jump(void* sender)
{
	Level* lvl=(Level*)sender;	
	if(lvl->jump_timeout<=0) // prevent callback 2 frames in a row
	{		
		lvl->player->charge_a(0, - 4);
		lvl->jump_timeout=1;
	}	
	else lvl->jump_timeout--;
	return 1; // Missing this line results in Undefined Opcode!
}

int Level::level_completed_dialog_finished(void* sender)
{		
	Level* lvl=(Level*)sender;
	lvl->unlock_input();	
	
	u8 option=lvl->dialog->get_option(0);
	
	if(option==1)
		lvl->completed=LVL_MENU;
	else 
		lvl->completed=LVL_NEXT;
	return 1;
}


int Level::game_over_dialog_finished(void* sender)
{		
	Level* lvl=(Level*)sender;
	lvl->unlock_input();
	lvl->completed=true;	
	
	u8 option=lvl->dialog->get_option(0);
	
	if(option==1)
		lvl->completed=LVL_MENU;
	else
		lvl->completed=LVL_RETRY;
	return 1;
}
