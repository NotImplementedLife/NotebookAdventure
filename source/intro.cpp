#include "intro.hpp"

#include "notebook-sheet.h"
#include "intro-fg.h"
#include "emojis.h"

#include <string.h>

class IntroBackgroundPage : public Background
{
public:
	IntroBackgroundPage() : Background(3,3,31,75,105) { strcpy(magic,"IntroBgPage"); }
	
	void init() override
	{
		Background::load_tiles(notebook_sheetTiles,notebook_sheetTilesLen, true, 0);
		Background::set_map_stream_source(notebook_sheetMap);
		dmaCopy(notebook_sheetPal,TMP_BG_PALETTE,notebook_sheetPalLen);				
	}	
};

class IntroForegroundPage : public Background
{
public:
	IntroForegroundPage() : Background(2,1,30,30,20) { strcpy(magic,"IntroFgPage");}
	
	void init() override
	{
		Background::load_tiles(intro_fgTiles,intro_fgTilesLen, true, 160);
		Background::set_map_stream_source(intro_fgMap);
		dmaCopy(intro_fgPal,TMP_BG_PALETTE+ 160,intro_fgPalLen);
	}	
};

#define CAT        __CAT
#define PLAYER     __PLR
#define SEL(x)     __SEL, x
#define MOV(x, y)  __MOV, x, y
#define SHOW       __SHW
#define HIDE       __HID
#define FLIP       __FLP
#define PAUSE(n)   __NOP, n
#define DONE       __DONE
#define PAW_EMOJI  __EMP
#define WARN_EMOJI __EMW
#define SPEED      __SPD

Intro::Intro()
{
	
}

void Intro::init()
{
	LOAD_GRIT_SPRITE_TILES(emojis, 0x0180, 0xB0, TMP_SPRITE_PALETTE);
	Background* bg = new IntroBackgroundPage();
	Background* fg = new IntroForegroundPage();
	
	set_background(3, bg, 0x10);
	set_background(2, fg, 0x00);
	
	camera->move(450,146);
	
	cat = new Cat();
	player = new Player();
	
	emoji_paw = quick_sprite(0x180, SIZE_32x32, ANCHOR_CENTER);
	emoji_warn = quick_sprite(0x1A0, SIZE_32x32, ANCHOR_CENTER);
		
	register_sprite(emoji_paw);
	register_sprite(emoji_warn);
	register_sprite(player);
	register_sprite(cat);
	
	cat->set_pos(450,216);
	player->set_pos(530,180);
	
	player->attr->hide();
	emoji_warn->attr->flip_h();
	emoji_warn->attr->hide();
	emoji_paw->attr->hide();
}

void Intro::on_key_down(int keys)
{
	if(keys & KEY_START)
	{
		this->exit(1);
	}
}
	
static const int __DONE =  0;
static const int __SEL  =  1;
static const int __MOV  =  2;
static const int __SHW  =  3;
static const int __HID  =  4;
static const int __CAT  =  5;
static const int __PLR  =  6;
static const int __NOP  =  7;
static const int __FLP  =  8;
static const int __EMW  =  9;
static const int __EMP  = 10;
static const int __SPD  = 11;
	
void Intro::on_frame()
{
	emoji_paw->set_pos(player->pos_x-18, player->pos_y-32);
	emoji_warn->set_pos(player->pos_x+18, player->pos_y-32);
	if(mvdelay>0) 
	{
		mvdelay--; return;
	}
	if(mvx!=0 || mvy!=0)
	{
		if(mvx>0) 
		{
			mva->attr->set_flip_h(1), mva->move(speed,0), mvx-=1;					
		}
		else if(mvx<0) mva->attr->set_flip_h(0), mva->move(-speed,0), mvx+=1;
		
		if(mvy>0) mva->move(0,speed), mvy-=1;		
		else if(mvy<0) mva->move(0,-speed), mvy+=1;
		
		if(mvx==0 && mvy==0)
		{
			mva->get_visual()->set_animation_track(ANIM_FRAMES_0);
		}
		
		return;		
	}
	int action = *(script++);
	switch(action)
	{
		case __DONE: this->exit(2); break;
		case __SEL : 
		{
			int target = *(script++);
			switch(target)
			{
				case __CAT : mva = cat; break;
				case __PLR : mva = player; break;
				case __EMP : mva = emoji_paw; break;
				case __EMW : mva = emoji_warn; break;
			}						
			break;
		}
		case __MOV:
		{
			mvx = *(script++);
			mvy = *(script++);
			mva->get_visual()->set_animation_track(ANIM_FRAMES_1);
			break;
		}
		case __SHW: mva->attr->show(); break;		
		case __HID:	mva->attr->hide(); break;		
		case __FLP: mva->attr->flip_h(); break;
		case __SPD: speed<<=1; break;
		case __NOP:
		{
			mvdelay = *(script++);
			break;
		}
	}	
}

const int INTRO_SCRIPT[] = 
{	
	PAUSE(50),
	SEL(CAT),
	MOV(35, 0),
	PAUSE(120),
	MOV(-35, 0),
	PAUSE(100),
	
	MOV(20, 0),
	PAUSE(150),
	
	MOV(-200, 0),
	SEL(PAW_EMOJI),
	SHOW,
	PAUSE(50),	
	
	SEL(PLAYER), 
	FLIP,
	SHOW,	
	PAUSE(100),
	
	MOV(0,27),
	PAUSE(70),
	FLIP,
	PAUSE(70),
	FLIP,
	PAUSE(70),
	SEL(PAW_EMOJI),
	HIDE,
	SEL(WARN_EMOJI),
	SHOW,
	PAUSE(50),
	SEL(PLAYER),
	FLIP,
	PAUSE(70),
	SPEED,
	MOV(-120,0),		
	
	PAUSE(100),
	DONE
};



