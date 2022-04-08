#include "title_screen.hpp"

#include <string.h>

#include "debug.hpp"

#include "notebook-sheet.h"
#include "title.h"
#include "digits.h"
#include "title_text.h"
#include "title_crayon.h"
#include "game_dat.hpp"
#include "audio.hpp"

#include "debug.hpp"

class TitleBackgroundPage : public Background
{
public:
	TitleBackgroundPage() : Background(3,2,31,75,105) { strcpy(magic,"TitleBgPage"); DEBUG_MSG("iTBG\n"); }
	
	void init() override
	{
		Background::load_tiles(notebook_sheetTiles,notebook_sheetTilesLen, true, 0);
		Background::set_map_stream_source(notebook_sheetMap);
		dmaCopy(notebook_sheetPal,TMP_BG_PALETTE,notebook_sheetPalLen);				
	}	
};

class TitleForegroundPage : public Background
{
public:
	TitleForegroundPage() : Background(2,1,30,30,20) { strcpy(magic,"TitleFgPage"); DEBUG_MSG("iTFG\n");}
	
	void init() override
	{
		Background::load_tiles(titleTiles,titleTilesLen, true, 160);
		Background::set_map_stream_source(titleMap);
		dmaCopy(titlePal,TMP_BG_PALETTE+ 160,titlePalLen);
	}	
};

class Digit : public Sprite
{
public:
	Digit() : Sprite(SIZE_8x16, 1)
	{		
		get_visual()->set_frame(0,0x120);
		get_visual()->set_crt_gfx(0);
		update_visual();
		set_anchor(ANCHOR_CENTER);
	}	
	void set_digit(u8 dig)
	{
		get_visual()->set_frame(0,0x120+4*dig);		
	}
};


TitleScreen::TitleScreen() { }

void TitleScreen::update_sel_level(s8 d)
{	
	sel_level+=d;	
	u8 max_level=UserData.current_level;
	if(((s8)sel_level)>=max_level)
		sel_level = max_level;
	if(((s8)sel_level)<1)
		sel_level = 1;
	digit1->set_digit(sel_level/10);
	digit2->set_digit(sel_level%10);
}

void TitleScreen::init()
{
	TitleBackgroundPage* bg_page = new TitleBackgroundPage();	
	set_background(3, bg_page, 0x10);
	
	TitleForegroundPage* fg_page = new TitleForegroundPage();
	set_background(2, fg_page, 0x00);	
	
	camera->move(300,146);
	
	LOAD_GRIT_SPRITE_TILES(title_text, 0x0008, 0, TMP_SPRITE_PALETTE);
	LOAD_GRIT_SPRITE_TILES(title_crayon, 0x0100, 68, TMP_SPRITE_PALETTE);
	LOAD_GRIT_SPRITE_TILES(digits, 0x0120, 0x50, TMP_SPRITE_PALETTE);
	
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
	
	Sprite* opt_level = new Sprite(SIZE_64x32,1);
	opt_level->get_visual()->set_frame(0,0x088);
	opt_level->get_visual()->set_crt_gfx(0);
	opt_level->update_visual();	
	opt_level->set_anchor(ANCHOR_LEFT);		
	opt_level->set_pos(350,192);
	register_sprite(opt_level);
	
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
	options[2]=opt_level;
	
	digit1 = new Digit();
	digit1->set_pos(392, 192);
	digit2 = new Digit();
	digit2->set_pos(400, 192);
	
	register_sprite(digit1);
	register_sprite(digit2);
	
	sel_level=UserData.current_level;
	update_sel_level(0);
	
	play_bgm(MOD_NADV_INTRO);
	mmSetModuleVolume(128);
	mmSetModulePitch(512+256);		
}

void TitleScreen::on_key_down(int keys)
{
	if(keys &  KEY_UP)
	{		
		if(crt_option>0) 
		{			
			if(crt_option==2 || crt_option==3)
				swap_digits_rb();
			crt_option--;		
			Sprite* opt = options[crt_option];
			crayon->set_pos(opt->pos_x,opt->pos_y);
		}
	}
	else if(keys & KEY_DOWN)
	{		
		if(crt_option<options_cnt-1)
		{	
			if(crt_option==1 || crt_option==2)
				swap_digits_rb();
			crt_option++;
			Sprite* opt = options[crt_option];
			crayon->set_pos(opt->pos_x,opt->pos_y);
		}
	}
	else if(keys & KEY_LEFT) 
	{		
		if(crt_option==2) update_sel_level(-1);
	}
	else if(keys & KEY_RIGHT)
	{		
		if(crt_option==2) update_sel_level(+1);
	}
	else if(keys & KEY_L) 
	{		
		if(crt_option==2) update_sel_level(-10);
	}
	else if(keys & KEY_R)
	{		
		if(crt_option==2) update_sel_level(+10);
	}
	else if(keys & KEY_A)
	{		
		//mmSetModuleVolume(64);
		play_sfx(&sfx_enter_lvl);
		for(int k=60;k--;)
		{
			VBlankIntrWait();
			mmFrame();
		}			
		switch(crt_option)
		{
			case 0: // New Game
			{
				reset_user_data();
				this->exit(LVL_ENTER_CODE(1)); break;
			}
			case 1: this->exit(LVL_ENTER_CODE(UserData.current_level)); break;
			case 2: this->exit(LVL_ENTER_CODE(sel_level)); break;
			default: fatal("Unrecognized menu option");
		}				
	}
}

void TitleScreen::on_frame()
{
	mmFrame();
}

TitleScreen::~TitleScreen()
{		
	stop_bgm();
}

void TitleScreen::swap_digits_rb()
{
	u16* pal=((u16*)SPRITE_PALETTE) + 0x50;
	for(int i=0;i<15;i++)
	{
		u16 cl=pal[i];
		u16 r = cl&0x1F;
		u16 g = (cl>>5)&0x1F;
		u16 b = (cl>>10)&0x1F;
		pal[i]=RGB5(b,g,r);
	}
}












