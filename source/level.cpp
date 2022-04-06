#include "level.hpp"

#include "debug.hpp"

#include "audio.hpp"

#include "game_dat.hpp"

#include "all_levels.h"

#include "level_1_bin.h"
#include "level_2_bin.h"
#include "level_3_bin.h"
#include "level_4_bin.h"
#include "level_5_bin.h"
//...

const int LEVELS_COUNT = all_levelsMapLen/(2*75*105);

const void* get_level_map(u32 no)
{	
	if(no>LEVELS_COUNT)
		fatal("Wrong level id");
	return (u16*)(all_levelsMap) + 75*105*(no-1);
}


const u8* levels_bin[] = { NULL, level_1_bin, level_2_bin, level_3_bin, level_4_bin, level_5_bin };

#include "notebook-sheet.h"
#include "player.h"
#include "cat.h"
#include "dialog_frame.h"
#include "dialog_arr.h"
#include "spikes.h"
#include "trampoline.h"

#include "obstacle_activator.h"
#include "obstacle_horizontal.h"
#include "obstacle_vertical.h"

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
	LevelDungeon(const void* level_map) : Background(2,0,29,75,105)
	{ 
		strcpy(magic,"LevelDungeon"); 
		Background::set_map_stream_source(level_map);
	}
	
	void init() override
	{				
		Background::load_tiles(all_levelsTiles, all_levelsTilesLen, true, 160);
		dmaCopy((u8*)all_levelsPal,(u16*)(BG_PALETTE)+160, all_levelsPalLen);
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
		get_visual()->set_frame(4,0x0088);
		get_visual()->set_frame(5,0x00A8);
		get_visual()->set_frame(6,0x00C8);
		get_visual()->set_frame(7,0x00E8);
		
		get_visual()->set_ticks(16);
		
		get_visual()->set_crt_gfx(0);	
		get_visual()->set_animation_frames(ANIM_FRAMES_0, 0,1,2,3, -1);
		get_visual()->set_animation_frames(ANIM_FRAMES_1, 0,4,5,6,7, -1);
		get_visual()->set_animation_track(ANIM_FRAMES_0);
		update_visual();
		//set_hitbox(10,0,14,30);
		set_hitbox(14,0,4,30);
		set_anchor(ANCHOR_BOTTOM);		
	}
};

class Cat : public PhysicalObject
{
private:

public:
	Cat() : PhysicalObject(SIZE_16x16, 16)
	{
		LOAD_GRIT_SPRITE_TILES(cat, 0x120, 128);
			
		get_visual()->set_frame(0,0x0120);		
		get_visual()->set_frame(1,0x0128);		
		get_visual()->set_frame(2,0x0130);		
		get_visual()->set_frame(3,0x0138);		
		get_visual()->set_frame(4,0x0140);		
		get_visual()->set_frame(5,0x0148);
		get_visual()->set_frame(6,0x0150);
		
		get_visual()->set_ticks(16);
		
		get_visual()->set_crt_gfx(0);	
		get_visual()->set_animation_frames(ANIM_FRAMES_0, 0, 1, 2, 3, -1);
		get_visual()->set_animation_frames(ANIM_FRAMES_1, 0, 4, 5, 6, -1);
		get_visual()->set_animation_track(ANIM_FRAMES_0);
		update_visual();		
		set_hitbox(3,0,9,16);
		set_anchor(ANCHOR_BOTTOM);
	}
};

class Explorer : public Sprite
{
public:
	Explorer() : Sprite(SIZE_8x8, 1, "explorer")
	{
		get_visual()->set_frame(0,0);
		get_visual()->set_crt_gfx(0);
		update_visual();		
		set_anchor(ANCHOR_CENTER);
	}
};

class Spikes : public Sprite
{
public:
	Spikes(ObjSize size) : Sprite(size, 1, "spikes")
	{
		get_visual()->set_frame(0,0x0200);
		get_visual()->set_crt_gfx(0);
		update_visual();
		auto_detect_hitbox();
		set_anchor(ANCHOR_BOTTOM_LEFT);
	}
};

class Trampoline : public Sprite
{
public:
	Trampoline() : Sprite(SIZE_16x16, 2, "tram")
	{
		get_visual()->set_frame(0,0x0208);
		get_visual()->set_frame(1,0x0210);
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
	Vwf *vwf_jp;
	Vwf *vwf_jc;
	Vwf *vwf_g;
public:
	LevelDialog() : DialogBackground(0, 1, 28) { strcpy(magic,"LvlDialog"); }
	
	void init() override
	{		
		set_priority(0);
		clear_map();
		load_tiles(dialog_frameTiles);
		dmaCopy(dialog_arrTiles, SPR_VRAM(0x21E), 64);
		set_caret(0x21E);
		
		// Dialog 0: use to display level title
		dbg_ctx="Vwf";
		vwf = new Vwf(defaultFont816);
		vwf->set_text_color(0x62);
		create_dialog_box(1, 1, 16, 4, vwf);		
		
		// Dialog 1: use dialog timings to control jumping on sprites
		// uses:
		// - trampoline (jump on short sprites)
		// - floating object simulation (jump on tall sprites)
		dbg_ctx="Vwf";
		vwf_jp = new Vwf(defaultFont816);
		create_dialog_box(0, 22, 3, 4, vwf_jp);
		
		// Dialog 2: general purpose user-involving dialog
		// used to display the "game over" message
		dbg_ctx="Vwf";
		vwf_g = new Vwf(defaultFont816);
		create_dialog_box(1,14,28,6, vwf_g);
		vwf_g->set_text_color(0x62);

		// Dialog 3: jump dialog, for cat
		dbg_ctx="Vwf";
		vwf_jc = new Vwf(defaultFont816);
		create_dialog_box(5, 22, 3, 4, vwf_jc);			
		
		BG_PALETTE[0x62] = 0x7FFF;
	}	
	
	virtual ~LevelDialog()
	{		
		dbg_ctx="Vwf";
		delete vwf;  
		dbg_ctx="Vwf";
		delete vwf_jp;
		dbg_ctx="Vwf";
		delete vwf_g;
		dbg_ctx="Vwf";
		delete vwf_jc;
	}
};

enum ObstacleOrientation
{
	horizontal_to_left  = 0x00,
	horizontal_to_right = 0x80,
	vertical_to_top     = 0x01,
	vertical_to_bottom  = 0x81
};

class Obstacle : public Sprite
{
private:
	u8 id; 
	u8 orientation;
	s16 relmove=0;
public:
	Obstacle(u8 id, ObstacleOrientation orientation) : Sprite(((orientation & 1) == 0) ? SIZE_32x8 : SIZE_8x32, 1, "obs")
	{
		this->id = id;
		this->orientation = orientation;
		get_visual()->set_frame(0,0x240 + 0x20*id + 8*(((u8)orientation) & 0x01));
		get_visual()->set_crt_gfx(0);
		update_visual();
		switch(orientation)
		{
			case horizontal_to_left: set_anchor(28*8, 128); break;
			case horizontal_to_right: set_anchor(4*8, 128); break;
			case vertical_to_top: set_anchor(128, 28*8); break;
			case vertical_to_bottom: set_anchor(128, 4*8); break;
		}		
	}			
	
	s8 get_id() const { return id; }

	u16 qopt_id=-1;
	
	void update_qopt(u16* qopt)
	{
		u16* addr=qopt+qopt_id;
		
		u8 x=((s16)pos_x)/8;
		u8 y=((s16)pos_y)/8;
		
		u8 dx=0, dy=0;
		if(orientation&0x01) dy=1; else dx=1;
		if((orientation&0x80)==0)
		{
			dx=-dx; dy = -dy;
		}
		
		for(int i=4;i--;)
		{
			*(addr++)=(y<<8)|x;
			x+=dx;
			y+=dy;
		}			
	}
	
	void spec_move(int direction)
	{
		s8 dx=0, dy=0;
		if(orientation&0x01) dy=-1; else dx=-1;
		if((orientation&0x80)==0)
		{
			dx=-dx; dy = -dy;
		}		
		if(direction>0 && relmove<32)
		{
			relmove++;			
			move(dx,dy);
		}
		else if(direction<0 && relmove>0)
		{
			relmove--;			
			move(-dx,-dy);		
		}
			
	}
};

class ObstacleActivator : public Sprite
{
private:
	u8 id;		
public:	
	ObstacleActivator(u8 id) : Sprite(SIZE_16x8, 2, "act")
	{
		this->id = id;		
		get_visual()->set_frame(0,0x240 + 0x20*id + 0x10);
		get_visual()->set_frame(1,0x240 + 0x20*id + 0x14);
		get_visual()->set_crt_gfx(0);
		update_visual();
		set_anchor(ANCHOR_BOTTOM);
	}	

	u8 get_id() const { return id; }
};

//////////////////////////////////////////////////////////////////////////////////////////

Level::Level(const void* lvl_map, const u8* lvl_src) : TextScrollMap()
{
	map_source = lvl_map;
	set_blocks_data(lvl_src);
	cat = NULL;	
	completed=false;
	for(int i=0;i<14;i++) obst_status[i]=0;
}

Level::Level(u32 level_no) : Level(get_level_map(level_no), levels_bin[level_no]) 
{
	id = level_no;
}

void Level::init() 
{			
	LOAD_GRIT_SPRITE_TILES(spikes, 0x200, 48);
	LOAD_GRIT_SPRITE_TILES(trampoline, 0x208, 56);	

	LevelBackgroundBage* bg_page = new LevelBackgroundBage();
	set_background(3, bg_page, 0x10);
	
	LevelDungeon* dungeon = new LevelDungeon(map_source);
	set_background(2, dungeon, 0x10);	
	
	dbg_ctx="LevelDialog";
	dialog = new LevelDialog();
	set_background(0, dialog, 0x00);
	
	dialog->run_on_dialog_finished(1, dialog_controlled_jump_p, this);
	dialog->run_on_dialog_finished(3, dialog_controlled_jump_c, this);
	
	REG_BLDCNT = (1<<2) | (1<<6) | (1<<11);
	REG_BLDALPHA = 13 | (3<<8);
	
	dbg_ctx="Explorer";
	explorer = new Explorer();
	register_sprite(explorer);
	
	dbg_ctx="Player";
	player = new Player();		
		
	u8* lvldat = (u8*)blocks_data+75*105+16;
	
	
	u16 px=0, py=0;
	px = (*(lvldat++));
	px |= (*(lvldat++))<<8;
	
	py = (*(lvldat++));
	py |= (*(lvldat++))<<8;
	
	player->set_pos(px, py);		
	register_sprite(player);			
		
	u16 cx=0, cy=0;	
	cx = (*(lvldat++));
	cx |= (*(lvldat++))<<8;
		
	cy = (*(lvldat++));
	cy |= (*(lvldat++))<<8;	
	
	dbg_ctx="Cat";
	cat	= new Cat();
	cat->set_pos(cx,cy);
	register_sprite(cat);
		
	u8 spkcnt = *(lvldat++);
	DEBUG_MSG("SPIKESCNT = %i\n", spkcnt);
	for(int i=0;i<spkcnt;i++)
	{
		u8 spkx = *(lvldat++);
		u8 spky = *(lvldat++);
		u8 spkl = *(lvldat++);
		DEBUG_MSG("%i %i %i", spkx, spky, spkl);
		mmSetModuleVolume(spkx+spky+spkl);
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
	
	play_bgm(MOD_NADV_LEVEL);
	mmSetModuleVolume(512+256);
	mmSetModulePitch(1024);
	
	// load obstacles
	
	for(int i=0;i<16;i++)
	{
		SPRITE_PALETTE[0xD0+i]=SYS_COLORS[i];
		SPRITE_PALETTE[0xE0+i]=cl_brighten(SYS_COLORS[i],8);
		SPRITE_PALETTE[0xF0+i]=cl_brighten(SYS_COLORS[i],14);
	}
			
	for(int i=0;i<14;i++)
	{
		u32 obj_id = 0x240+0x20*i;
		dmaCopy( obstacle_horizontalTiles,(u8*)SPR_VRAM(obj_id), obstacle_horizontalTilesLen);
		dmaCopy( obstacle_verticalTiles,(u8*)SPR_VRAM(obj_id+0x08), obstacle_verticalTilesLen);
		dmaCopy( obstacle_activatorTiles,(u8*)SPR_VRAM(obj_id+0x10), obstacle_activatorTilesLen);
		u32* obj_addr = SPR_VRAM(obj_id);
		for(int j=0;j<0x80;j++)
		{			
			u32 val = obj_addr[j];
			u32 res = 0;
			
			for(int k=0;val>0;k++)
			{				
				u8 b = val&0xFF;
				val>>=8;
				if(b>0)
				{
					b<<=4;
					b= 0x100-b+i+1;
				}		
				res|=(b<<(k<<3));				
			}
			
			obj_addr[j]=res;			
		}
		
		obj_addr+=0x80;
		for(int j=0;j<0x40;j++)
		{			
			u32 val = obj_addr[j];
			u32 res = 0;
			
			for(int k=0;val>0;k++)
			{				
				u8 b = val&0xFF;
				val>>=8;
				switch(b)
				{
					case 0: break;
					case 1: b=0xD0; break;
					case 3: b=0xFF; break;
					default:
					{
						b<<=4;
						b= 0x100-b+i+1;
					}					
				}				
				res|=(b<<(k<<3));				
			}
			
			obj_addr[j]=res;			
		}
	}
	
	u8 obscnt = *(lvldat++);
	
	qopt_size=4*obscnt;
	//quick_obstacle_pos_table = new u16[qopt_size];
	
	for(int i=0;i<obscnt;i++)
	{
		u8 obsid = *(lvldat++);
		u8 obso = *(lvldat++);
		u8 obsx = *(lvldat++);
		u8 obsy = *(lvldat++);		
		add_obstacle(obsid,obso,(obsx<<3)+4,(obsy<<3)+4,4*i);
	}
	
	
	u8 actcnt = *(lvldat++);
	for(int i=0;i<actcnt;i++)
	{
		u8 aid = *(lvldat++);
		u8 ax = *(lvldat++);
		u8 ay = *(lvldat++);
		add_obstacle_activator(aid,ax<<2, (ay+1)<<3);
	}
}

void Level::update_actor(PhysicalObject* obj)
{	
	u8 bdata = get_block_data((int)obj->get_pos_x(), (int)obj->get_pos_y());	
	if(bdata==0)
	{		
		u8 y0=((int)obj->get_pos_y())>>3;
		u8 ymin=y0, ymax=y0;
		u8 x0=((int)obj->get_pos_x())>>3;
				
		for(int y=y0;y<104;y++)
			if(get_block_data_tiled(x0,y))
			{
				ymax=y;
				break;
			}			
		for(int y=y0;y>0;y--)
			if(get_block_data_tiled(x0,y))
			{
				ymin=y;
				break;
			}							
		obj->set_air_limits((ymin+1)<<3,ymax<<3);							
		obj->apply_g();
	}
	else
	{		
		u8 udata = get_block_data((int)obj->get_pos_x(), (int)obj->get_pos_y()-1);		
		obj->ignore_air_limits();		
		if(bdata==1 || (bdata==0 && udata==2))
		{
			// 8 multiple correction
			obj->pos_y = ((int)obj->pos_y)&(~7);
		}
		if(bdata==2)
		{
			obj->ay=0;
		}		
	}
	
	obj->render();
}

void Level::on_frame() 
{	
	if(completed)
	{
		this->exit(completed);
		return;
	}
		
	if(focus==player) {
		UserData.time_played_as_human++;
	}
	else if(focus==cat) {
		UserData.time_played_as_cat++;
	}
	
	update_actor(player);
	update_actor(cat);
	
	for(int i=0;i<sprites_count;i++)
	{
		if(sprites[i]->is_of_class("spikes"))
		{
			if(player->touches(sprites[i]) || cat->touches(sprites[i]))
			{		
				if(!input_locked())
				{
					lock_input(0);
					dialog->run_on_dialog_finished(2, game_over_dialog_finished, this);
					dialog->launch_dialog(2, "Game over!\n                 \x01Retry            \01Menu", 0);
					mmStop();
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
			if(cat->touches(sprites[i]))
			{
				sprites[i]->get_visual()->set_animation_track(ANIM_FRAMES_0,false);
				dialog->launch_dialog(3,"i",10);
			}
		}
		else if(sprites[i]->is_of_class("act"))
		{
			int oid=((ObstacleActivator*)sprites[i])->get_id();
			if(player->touches(sprites[i]) || cat->touches(sprites[i]))
			{				
				sprites[i]->get_visual()->set_crt_gfx(1);				
				obst_status[oid]=1;
				//((ObstacleActivator*)sprites[i])->spec_move(1);
			}
			else
			{
				sprites[i]->get_visual()->set_crt_gfx(0);
				obst_status[oid]=-1;
				//((ObstacleActivator*)sprites[i])->spec_move(-1);
			}
		}		
		else if(sprites[i]->is_of_class("obs"))
		{
			Obstacle* obs = (Obstacle*)sprites[i];
			obs->spec_move(obst_status[obs->get_id()]);
			obs->update_qopt(quick_obstacle_pos_table);
		}
		else if(player->touches(cat))
		{
			if(!input_locked())
			{
				lock_input(0);
				dialog->run_on_dialog_finished(2, level_completed_dialog_finished, this);
				dialog->launch_dialog(2, "Well played!\n                  \x01Next            \01Menu", 0);			
				mmStop();
			}
		}
	}
}

void Level::on_end_frame()
{	
	mmFrame();
}

void Level::on_key_down(int keys)
{	
	if(focus!=explorer)
	{				
		if(keys & KEY_A) 
		{
			s16 px=(int)xfocus->get_pos_x();
			s16 py=(int)xfocus->get_pos_y();
			//u8 ddata = get_block_data(px, py+8);
			u8 bdata = get_block_data(px, py);
			u8 udata = get_block_data(px, py-8);
			if(bdata!=0 && udata==0)			
				xfocus->charge_a(0,-sf24(3,160));
		}
		
		if(keys & KEY_START)
		{
			//xfocus->ignore_air_limits();
			//xfocus->set_pos(100,100);			
			lock_execution(0);
			dialog->run_on_dialog_finished(2, pause_dialog_finished, this);
			dialog->launch_dialog(2, "Game paused\n    \x01Resume          \01Retry          \01Menu", 0);
			mmPause();
			
		}
		
		if(keys & KEY_L)
		{
			xfocus->get_visual()->set_animation_track(ANIM_FRAMES_0);
			set_focus(focus==cat ? player:cat);		
			if(keysHeld() & (KEY_LEFT|KEY_RIGHT))
			{
				xfocus->get_visual()->set_animation_track(ANIM_FRAMES_1);
			}
		}
		if(keys & KEY_R)
		{
			explorer->set_pos(xfocus->pos_x,xfocus->pos_y);
			set_focus(explorer);
		}
		
		if(keys & (KEY_LEFT|KEY_RIGHT)) 
		{
			xfocus->get_visual()->set_animation_track(ANIM_FRAMES_1);			
		}
	}	
}

void Level::on_loaded()
{
	dialog->launch_dialog(0,(const char*)blocks_data+75*105, 60);	
}

void Level::on_key_held(int keys)
{			
	if(focus!=explorer)
	{		
		if(keys & KEY_DOWN)  
		{	
			int block = get_block_data((int)(xfocus->pos_x),(int)xfocus->get_pos_y());
			if(block==2)
			{				
				xfocus->move(0,1);
			}
		}
		else if(keys & KEY_UP)  
		{
			int upper = get_block_data((int)(xfocus->pos_x),(int)xfocus->get_pos_y()-1);		
			if(upper==2)
			{			
				xfocus->move(0,-1);
			}
		}
		
		
		if(keys & KEY_LEFT) 
		{			
			xfocus->attr->set_flip_h(0);					
			
			int on_left = get_block_data((int)(xfocus->get_left_coord())-1,(int)xfocus->get_pos_y()-2);
			on_left |= get_block_data((int)(xfocus->get_left_coord())-1,(int)xfocus->get_pos_y()-10);
			if(xfocus==player)
			{
				on_left |= get_block_data((int)(xfocus->get_left_coord())-1,(int)xfocus->get_pos_y()-18);
				on_left |= get_block_data((int)(xfocus->get_left_coord())-1,(int)xfocus->get_pos_y()-26);
			}
			
			if(on_left!=1)
				xfocus->charge_v(-sf24(2),0);		
		}
		else if(keys & KEY_RIGHT) 
		{			
			xfocus->attr->set_flip_h(1);
			
			int on_right = get_block_data((int)(xfocus->get_right_coord())+1,(int)xfocus->get_pos_y()-2);
			on_right |= get_block_data((int)(xfocus->get_right_coord())+1,(int)xfocus->get_pos_y()-10);
			if(xfocus==player)
			{
				on_right |= get_block_data((int)(xfocus->get_right_coord())+1,(int)xfocus->get_pos_y()-18);
				on_right |= get_block_data((int)(xfocus->get_right_coord())+1,(int)xfocus->get_pos_y()-26);
			}
			
			if(on_right!=1)
				xfocus->charge_v(sf24(2),0);		
		}	
	}	
	else
	{
		if(keys & KEY_DOWN)  
		{
			explorer->move(0,2);
		}
		else if(keys & KEY_UP)
		{
			explorer->move(0,-2);
		}
		
		if(keys & KEY_LEFT)  
		{
			explorer->move(-2,0);
		}
		else if(keys & KEY_RIGHT)
		{
			explorer->move(2,0);
		}
	}
}

void Level::on_key_up(int keys)
{		
	if(focus==explorer)
	{
		if(keys & KEY_R)
		{
			set_focus(xfocus);
		}
	}
	else 
	{
		if(keys & (KEY_LEFT | KEY_RIGHT))
		{
			xfocus->get_visual()->set_animation_track(ANIM_FRAMES_0);
		}
	}
}

void Level::set_blocks_data(const u8* src)
{
	blocks_data=src;
}

u8 Level::get_block_data(s16 x, s16 y) const
{
	return get_block_data_tiled(x>>3,y>>3);
}

u8 Level::get_block_data_tiled(u8 x, u8 y) const
{
	u8 on_map = blocks_data[mul_75[y]+x];
	if(on_map)
		return on_map;
	// check obstacles
	u16 yx=(y<<8)|x;
	for(int i=0;i<qopt_size;i++)
	{
		if(yx==quick_obstacle_pos_table[i])
			return 1;
	}
	return 0;
}

void Level::set_focus(Sprite* spr)
{	
	focus=spr;	
	if(focus==player || focus==cat)
	{
		xfocus=(PhysicalObject*)spr;
	}
	camera->follow(focus);	
	focus->update_position(camera);	
}


void Level::add_spikes(s16 x, s16 y, s8 len)
{
	while(len>=4)
	{
		dbg_ctx="Spikes";
		Spikes* spikes = new Spikes(SIZE_32x8);
		spikes->set_pos(x,y);
		x+=32;
		len-=4;
		register_sprite(spikes);
	}
	while(len>=2)
	{
		dbg_ctx="Spikes";
		Spikes* spikes = new Spikes(SIZE_16x8);
		spikes->set_pos(x,y);
		x+=16;
		len-=2;
		register_sprite(spikes);
	}	
	while(len>=1)
	{
		dbg_ctx="Spikes";
		Spikes* spikes = new Spikes(SIZE_8x8);
		spikes->set_pos(x,y);
		x+=8;
		len-=1;
		register_sprite(spikes);
	}	
}

void Level::add_trampoline(s16 x, s16 y)
{
	dbg_ctx="Trampoline";
	Trampoline* tr = new Trampoline();
	tr->set_pos(x,y);
	register_sprite(tr);
}

void Level::add_obstacle(u8 id, u8 orientation, s16 x, s16 y, u16 qopt_id)
{
	dbg_ctx="Obstacle";
	Obstacle* o=new Obstacle(id, (ObstacleOrientation)orientation);
	o->set_pos(x,y);
	o->qopt_id=qopt_id;
	o->update_qopt(quick_obstacle_pos_table);
	register_sprite(o);
}

void Level::add_obstacle_activator(u8 id, s16 x, s16 y)
{
	dbg_ctx="Activator";
	ObstacleActivator* a = new ObstacleActivator(id);
	a->set_pos(x,y);
	register_sprite(a);
}

Level::~Level()
{
	stop_bgm();	
}

int Level::dialog_controlled_jump_p(void* sender)
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

int Level::dialog_controlled_jump_c(void* sender)
{
	Level* lvl=(Level*)sender;	
	if(lvl->jump_timeout<=0) // prevent callback 2 frames in a row
	{		
		lvl->cat->charge_a(0, - 4);
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
		lvl->completed=LVL_COMPLETE | LVL_MENU;
	else 
	{
		if(lvl->id==LEVELS_COUNT)
			lvl->completed=LVL_COMPLETE | LVL_MENU;
		else
			lvl->completed=LVL_COMPLETE | LVL_NEXT;
	}
	return 1;
}


int Level::game_over_dialog_finished(void* sender)
{		
	Level* lvl=(Level*)sender;
	lvl->unlock_input();
	lvl->completed=true;	
	
	u8 option=lvl->dialog->get_option(0);
	
	if(option==1)
		lvl->completed=LVL_MENU; // Quit
	else
		lvl->completed=LVL_ENTER_CODE(lvl->id); // Retry
	return 1;
}

int Level::pause_dialog_finished(void* sender)
{
	Level* lvl=(Level*)sender;
	lvl->unlock_execution();
	u8 option=lvl->dialog->get_option(0);
	if(option==0)
	{
		mmResume();
		return 1; // Resume
	}	
	if(option==1)
	{
		lvl->completed=LVL_ENTER_CODE(lvl->id); // Retry
		return 1;
	}
	if(option==2)
	{
		lvl->completed=LVL_MENU; // Quit
		return 1;
	}
	fatal("Undefined option");
	return 1;
}

const u16 mul_75[] = 
{
	0x00*75, 0x01*75, 0x02*75, 0x03*75, 0x04*75, 0x05*75, 0x06*75, 0x07*75, 0x08*75, 0x09*75, 0x0A*75, 0x0B*75, 0x0C*75, 0x0D*75, 0x0E*75, 0x0F*75,
	0x10*75, 0x11*75, 0x12*75, 0x13*75, 0x14*75, 0x15*75, 0x16*75, 0x17*75, 0x18*75, 0x19*75, 0x1A*75, 0x1B*75, 0x1C*75, 0x1D*75, 0x1E*75, 0x1F*75,
	0x20*75, 0x21*75, 0x22*75, 0x23*75, 0x24*75, 0x25*75, 0x26*75, 0x27*75, 0x28*75, 0x29*75, 0x2A*75, 0x2B*75, 0x2C*75, 0x2D*75, 0x2E*75, 0x2F*75,
	0x30*75, 0x31*75, 0x32*75, 0x33*75, 0x34*75, 0x35*75, 0x36*75, 0x37*75, 0x38*75, 0x39*75, 0x3A*75, 0x3B*75, 0x3C*75, 0x3D*75, 0x3E*75, 0x3F*75,
	0x40*75, 0x41*75, 0x42*75, 0x43*75, 0x44*75, 0x45*75, 0x46*75, 0x47*75, 0x48*75, 0x49*75, 0x4A*75, 0x4B*75, 0x4C*75, 0x4D*75, 0x4E*75, 0x4F*75,
	0x50*75, 0x51*75, 0x52*75, 0x53*75, 0x54*75, 0x55*75, 0x56*75, 0x57*75, 0x58*75, 0x59*75, 0x5A*75, 0x5B*75, 0x5C*75, 0x5D*75, 0x5E*75, 0x5F*75,
	0x60*75, 0x61*75, 0x62*75, 0x63*75, 0x64*75, 0x65*75, 0x66*75, 0x67*75, 0x68*75, 0x69*75, 0x6A*75, 0x6B*75, 0x6C*75, 0x6D*75, 0x6E*75, 0x6F*75,
	0x70*75, 0x71*75, 0x72*75, 0x73*75, 0x74*75, 0x75*75, 0x76*75, 0x77*75, 0x78*75, 0x79*75, 0x7A*75, 0x7B*75, 0x7C*75, 0x7D*75, 0x7E*75, 0x7F*75,
	0x80*75, 0x81*75, 0x82*75, 0x83*75, 0x84*75, 0x85*75, 0x86*75, 0x87*75, 0x88*75, 0x89*75, 0x8A*75, 0x8B*75, 0x8C*75, 0x8D*75, 0x8E*75, 0x8F*75,
	0x90*75, 0x91*75, 0x92*75, 0x93*75, 0x94*75, 0x95*75, 0x96*75, 0x97*75, 0x98*75, 0x99*75, 0x9A*75, 0x9B*75, 0x9C*75, 0x9D*75, 0x9E*75, 0x9F*75,
	0xA0*75, 0xA1*75, 0xA2*75, 0xA3*75, 0xA4*75, 0xA5*75, 0xA6*75, 0xA7*75, 0xA8*75, 0xA9*75, 0xAA*75, 0xAB*75, 0xAC*75, 0xAD*75, 0xAE*75, 0xAF*75,
	0xB0*75, 0xB1*75, 0xB2*75, 0xB3*75, 0xB4*75, 0xB5*75, 0xB6*75, 0xB7*75, 0xB8*75, 0xB9*75, 0xBA*75, 0xBB*75, 0xBC*75, 0xBD*75, 0xBE*75, 0xBF*75,
	0xC0*75, 0xC1*75, 0xC2*75, 0xC3*75, 0xC4*75, 0xC5*75, 0xC6*75, 0xC7*75, 0xC8*75, 0xC9*75, 0xCA*75, 0xCB*75, 0xCC*75, 0xCD*75, 0xCE*75, 0xCF*75,
	0xD0*75, 0xD1*75, 0xD2*75, 0xD3*75, 0xD4*75, 0xD5*75, 0xD6*75, 0xD7*75, 0xD8*75, 0xD9*75, 0xDA*75, 0xDB*75, 0xDC*75, 0xDD*75, 0xDE*75, 0xDF*75,
	0xE0*75, 0xE1*75, 0xE2*75, 0xE3*75, 0xE4*75, 0xE5*75, 0xE6*75, 0xE7*75, 0xE8*75, 0xE9*75, 0xEA*75, 0xEB*75, 0xEC*75, 0xED*75, 0xEE*75, 0xEF*75,
	0xF0*75, 0xF1*75, 0xF2*75, 0xF3*75, 0xF4*75, 0xF5*75, 0xF6*75, 0xF7*75, 0xF8*75, 0xF9*75, 0xFA*75, 0xFB*75, 0xFC*75, 0xFD*75, 0xFE*75, 0xFF*75,
};
