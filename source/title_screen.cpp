#include "title_screen.hpp"

#include <string.h>

#include "notebook-sheet.h"
#include "title.h"
#include "title_text.h"
#include "title_crayon.h"
#include "game_dat.hpp"

class TitleBackgroundPage : public Background
{
public:
	TitleBackgroundPage() : Background(3,2,31,75,105) { strcpy(magic,"TitleBgPage"); }
	
	void init() override
	{
		Background::load_tiles(notebook_sheetTiles,notebook_sheetTilesLen, true, 0);
		Background::set_map_stream_source(notebook_sheetMap);
		dmaCopy((u8*)notebook_sheetPal,(u8*)(BG_PALETTE),notebook_sheetPalLen);
	}	
};

class TitleForegroundPage : public Background
{
public:
	TitleForegroundPage() : Background(2,1,30,30,20) { strcpy(magic,"TitleFgPage"); }
	
	void init() override
	{
		Background::load_tiles(titleTiles,titleTilesLen, true, 160);
		Background::set_map_stream_source(titleMap);
		dmaCopy((u8*)titlePal,(u8*)(BG_PALETTE)+2*160,titlePalLen);
	}	
};


TitleScreen::TitleScreen() { }

void TitleScreen::init()
{
	TitleBackgroundPage* bg_page = new TitleBackgroundPage();	
	set_background(3, bg_page, 0x10);
	
	TitleForegroundPage* fg_page = new TitleForegroundPage();
	set_background(2, fg_page, 0x00);	
	
	camera->move(300,146);
	
	LOAD_GRIT_SPRITE_TILES(title_text, 0x0008, 0);
	LOAD_GRIT_SPRITE_TILES(title_crayon, 0x0100, 68);
	
	Sprite* opt_new = new Sprite(SIZE_64x32,1);
	opt_new->get_visual()->set_frame(0,0x008);
	opt_new->get_visual()->set_crt_gfx(0);
	opt_new->update_visual();	
	opt_new->set_anchor(ANCHOR_LEFT);		
	opt_new->set_pos(350,146);
	register_sprite(opt_new);
	
	Sprite* opt_cont = new Sprite(SIZE_64x32,1);
	opt_cont->get_visual()->set_frame(0,0x048);
	opt_cont->get_visual()->set_crt_gfx(0);
	opt_cont->update_visual();	
	opt_cont->set_anchor(ANCHOR_LEFT);		
	opt_cont->set_pos(350,168);
	register_sprite(opt_cont);
	
	crayon = new Sprite(SIZE_16x16,3);
	
	crayon->get_visual()->set_frame(0,0x0100);
	crayon->get_visual()->set_frame(1,0x0108);
	crayon->get_visual()->set_frame(2,0x0110);
	
	crayon->get_visual()->set_ticks(16);
	crayon->get_visual()->set_animation_frames(ANIM_FRAMES_0, 0,1,0, 2, -1);
	crayon->get_visual()->set_animation_track(ANIM_FRAMES_0);
	
	crayon->set_anchor(ANCHOR_RIGHT);
	
	register_sprite(crayon);

	crayon->set_pos(opt_new->pos_x,opt_new->pos_y);
	
	options[0]=opt_new;
	options[1]=opt_cont;
	
}

void TitleScreen::on_key_down(int keys)
{
	if(keys & (KEY_DOWN | KEY_UP))
	{
		crt_option^=1;
		Sprite* opt = options[crt_option];
		crayon->set_pos(opt->pos_x,opt->pos_y);
	}
	else if(keys & KEY_A)
	{
		if(crt_option==0) // new game
		{
			UserData.current_level = 1;
			UserData.time_played_as_human = 0;
			UserData.time_played_as_cat = 0;
		}
		exit(crt_option==1 ? LVL_ENTER_CODE(UserData.current_level) : LVL_ENTER_CODE(1));
	}
}













