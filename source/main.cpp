#include <gba.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine.hpp"

#include "scrollmap.hpp"
#include "notebook-sheet.h"
#include "dialog_frame.h"

#include "heart_sprite.h"
#include "heart_narrow.h"

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
		launch_dialog(0,"Test title 1",30);
		launch_dialog(1,"Days go past, and days come still, All is old and all is new, What is well and what is ill, You imagine and construe Do not hope and do not fear, Waves that leap like waves must fall; Should they praise or should they jeer,Look but coldly on it all.");
	}
};

class scrollmap : virtual public TextScrollMap
{
private:
	map3 *m3;
	Sprite *xheart1;
	Sprite *xheart2;
public:
	scrollmap() : TextScrollMap() {}
	
	void init() override
	{	
		((u32*)(VRAM))[0]++;
		TextScrollMap::init();
		
		m3 = new map3();		
		m3->set_scroll(0,0);		
		set_background(3, m3, 1);		
		
		SPRITE_PALETTE[1]=RGB5(31,0,0);
		
		LOAD_GRIT_SPRITE_TILES(heart_sprite, 8);
		LOAD_GRIT_SPRITE_TILES(heart_narrow, 16);		
		
		xheart1=new Sprite(SIZE_16x16, 1);
		xheart1->get_visual()->set_frame(0,8);
		xheart1->get_visual()->set_crt_gfx(0);
		xheart1->update_visual();
		xheart1->auto_detect_hitbox();
		xheart1->set_anchor(ANCHOR_CENTER);
		xheart2->set_pos(100,100);		
		
		xheart2=new Sprite(SIZE_16x16, 1);							
		xheart2->get_visual()->set_frame(0,16);
		xheart2->get_visual()->set_crt_gfx(0);
		xheart2->update_visual();
		xheart2->auto_detect_hitbox();
		xheart2->set_anchor(ANCHOR_CENTER);
		xheart2->set_pos(-16,00);
		
		get_camera()->follow(xheart1);
		
		xheart1->update_position(get_camera());
		xheart2->update_position(get_camera());
		
	}
	
	void on_frame() override
	{	
		//fatal("1",(u32)m3);
		//m3->build_map();
		xheart2->move(sf24(0,32),0);		
		xheart1->update_position(get_camera());
		xheart2->update_position(get_camera());
		
		if(xheart1->touches(xheart2))
		{
			SPRITE_PALETTE[1]=RGB5(0,31,0);		
		}
		else
			SPRITE_PALETTE[1]=RGB5(31,0,0);
		
		
	}	
};

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);
	SetMode(MODE_0 | OBJ_ENABLE | OBJ_1D_MAP);
		
	OamPool::reset();
		
	scrollmap* sm = new scrollmap();
	sm->init();
	sm->run();	
	
	delete sm;
}


