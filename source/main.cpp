#include <gba.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine.hpp"

#include "scrollmap.hpp"
#include "notebook-sheet.h"
#include "dialog_frame.h"

#include "heart_sprite.h"
#include "heart_narrow.h"
#include "player_sheet.h"

#include <string.h>

#include <math.h>

class map3 : public Background
{
public:
	map3() : Background(3,2,31,75,105) { strcpy(magic,"map3"); }
	
	void init() override
	{		
		Background::load_tiles(notebook_sheetTiles,notebook_sheetTilesLen,true,0);
		Background::set_map_stream_source(notebook_sheetMap);
		dmaCopy((u8*)notebook_sheetPal,(u8*)(BG_PALETTE),notebook_sheetPalLen);
	}	
};

#include "dialog_arr.h"

class map0 : public DialogBackground
{
private:
	Vwf *vwf1, *vwf2, *vwf3;	
public:
	map0() : DialogBackground(0, 0, 28) { strcpy(magic,"map0"); }
	
	void init() override
	{
		load_tiles(dialog_frameTiles);
		clear_map();
		vwf1=new Vwf(defaultFont816);
		vwf2=new Vwf(defaultFont816);
		vwf3=new Vwf(defaultFont816);
		create_dialog_box(1, 1, 16, 4, vwf1);
		create_dialog_box(1, 14, 28, 6, vwf2);
		create_dialog_box(1, 8, 28, 6, vwf3);
		
		dmaCopy(dialog_arrTiles, (u8*)(VRAM + 0x10000 + 0x64*64), 64);
		set_caret(0xC8);		
		
		vwf1->set_text_color(15);
		vwf2->set_text_color(15);		
		vwf3->set_text_color(15);
		launch_dialog(0,"Test title 1",200);
	}	
};

class scrollmap : public TextScrollMap
{
private:
	map0 *m0;
	//map3 *m3;
	Sprite *xheart1;
	Sprite *xheart2;
	Sprite *player;
	sf24 ax, ay;
	Sprite* focus;
public:
	scrollmap() : TextScrollMap() {}
	
	void init() override
	{			
		TextScrollMap::init();
		
		m0 = new map0();
		m0->set_scroll(0,0);		
		set_background(0, m0, 0x00);
		
		Background* m3 = new map3();
		//m3->set_scroll(0,0);		
		set_background(3, m3, 0x10);
		
		SPRITE_PALETTE[1]=RGB5(31,0,0);
		
		LOAD_GRIT_SPRITE_TILES(heart_sprite,  8, 1);
		LOAD_GRIT_SPRITE_TILES(heart_narrow, 16, 2);	
		LOAD_GRIT_SPRITE_TILES(player_sheet, 24, 8);
		
		xheart1=new Sprite(SIZE_16x16, 1);
		xheart1->get_visual()->set_frame(0,8);
		xheart1->get_visual()->set_crt_gfx(0);
		xheart1->update_visual();
		xheart1->auto_detect_hitbox();
		xheart1->set_anchor(ANCHOR_CENTER);
		xheart1->set_pos(100,500);
				
		
		xheart2=new Sprite(SIZE_16x16, 1);							
		xheart2->get_visual()->set_frame(0,16);
		xheart2->get_visual()->set_crt_gfx(0);
		xheart2->update_visual();
		xheart2->auto_detect_hitbox();
		xheart2->set_anchor(ANCHOR_CENTER);
		xheart2->set_pos(70,500);
		
		player=new Sprite(SIZE_16x16, 1);							
		player->get_visual()->set_frame(0,24);
		player->get_visual()->set_crt_gfx(0);
		player->update_visual();
		player->auto_detect_hitbox();
		player->set_anchor(ANCHOR_CENTER);
		player->set_pos(70,500);
		
		register_sprite(xheart1);
		register_sprite(xheart2);
		register_sprite(player);
		
		set_focus(player);
		
		//xheart1->update_position(camera);
		//xheart2->update_position(camera);
		//player->update_position(camera);
		
	}
	
	void set_focus(Sprite* spr)
	{
		focus = spr;
		camera->follow(spr);
	}
	
	void toggle_focus()
	{
		set_focus(focus==xheart1 ? xheart2: xheart1);
	}
	
	void on_frame() override
	{	
		focus->move(ax, ay);
		if(ax>0)	
			ax-=sf24(0,1);		
		else if(ax<0)
			ax+=sf24(0,1);
		
		if(ay>0)	
			ay-=sf24(0,1);		
		else if(ay<0)
			ay+=sf24(0,1);
		
		if(xheart1->touches(xheart2))		
			SPRITE_PALETTE[1]=RGB5(0,31,0);				
		else
			SPRITE_PALETTE[1]=RGB5(31,0,0);
	}	
	
	void on_key_down(int keys) override
	{
		if(keys & KEY_B) toggle_focus();
	}
	
	void on_key_held(int keys) override
	{		
		if(keys & KEY_UP) ay = -sf24(0,100);
		if(keys & KEY_DOWN) ay = sf24(0,100);
		
		if(keys & KEY_LEFT) ax = -sf24(0,100);
		if(keys & KEY_RIGHT) ax = sf24(0,100);
	}
};

#include "level.hpp"

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);
	SetMode(MODE_0 | OBJ_ENABLE | OBJ_1D_MAP);
		
	OamPool::reset();
		
	//scrollmap* sm = new scrollmap();
	//sm->init();
	//sm->run();	
	//delete sm;
	
	Level* lvl = new Level();
	lvl->init();
	lvl->run();
	delete lvl;
	
	
}


