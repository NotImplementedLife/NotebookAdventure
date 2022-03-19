#include <gba.h>
#include <stdio.h>
#include <stdlib.h>

#include "scrollmap.hpp"
#include "notebook-sheet.h"
#include "dialog_frame.h"
#include "vwf.hpp"
#include "sprite.hpp"
#include "error.hpp"

#include "heart_sprite.h"

#include <math.h>

class map3 : public Background
{
public:
	map3() : Background(3,2,31,75,105)
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
	Vwf *vwf1, *vwf2;
public:
	map0() : DialogBackground(0, 0, 28)
	{
		load_tiles(dialog_frameTiles);
		clear_map();
		vwf1=new Vwf(defaultFont816);
		vwf2=new Vwf(defaultFont816);
		create_dialog_box(1, 1, 16, 4, vwf1);
		create_dialog_box(1, 14, 28, 6, vwf2);
		
		dmaCopy(dialog_arrTiles, (u8*)(VRAM + 0x10000 + 0x64*64), 64);
		set_caret(0xC8);
		assign_caret_to(1);
		
		vwf1->set_text_color(15);
		vwf2->set_text_color(20);		
		vwf1->draw_text("Test title 1");
		launch_dialog(1,"Days go past, and days come still, All is old and all is new, What is well and what is ill, You imagine and construe Do not hope and do not fear, Waves that leap like waves must fall; Should they praise or should they jeer,Look but coldly on it all.");
	}	
};

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);

	SetMode(MODE_0 | OBJ_ENABLE | OBJ_1D_MAP);
	OamPool::reset();		
	
	dmaCopy(heart_spriteTiles,(u8*)(VRAM|0x10000)+4*64,heart_spriteTilesLen);
	
	//int obj1 = OamPool::add_obj(ObjAttribute(SIZE_16x16, 5, 6, 8, 0, 0, 2));
	
	//dmaCopy(OamPool::get_object_by_id(obj1), (u32*)VRAM, 8);
	
	
	//while (1) VBlankIntrWait();	
		
	SPRITE_PALETTE[1]=RGB5(31,0,0);	
	
	for(int i=0;i<10;i++)
		for(int j=0;j<i;j++)
		OamPool::add_obj(ObjAttribute(SIZE_16x16, 20*i, 20*j, 8, 0, 0, 0));
	
	
	OamPool::deploy();
	for(int i=0;i<10;i+=3)
		for(int j=0;j<i;j++)
	{
		ObjAttribute* obj = OamPool::get_object_by_id(i*(i-1)/2+j);
		obj->flip_v();
		obj->flip_h();
	}
	OamPool::deploy();
	
	for(int i=0;i<10;i++)
		for(int j=0;j<i;j++)
	{
		int id=i*(i-1)/2+j;
		if(id%3==1) OamPool::remove_obj(id);
	}
	OamPool::deploy();	
	
	map3 bg3;
	map0 bg0;	
	bg0.set_scroll(0,0);	
		
	OamPool::deploy();
	

	int x[300];
	int y[300];
	for(int t=0;t<300;t++)
	{
		double a=2*3.14159*t/300;
		x[t]=300+160*cos(a)-120;
		y[t]=420+160*sin(a)-80;
	}
	int t=0;
	while (1) {		
		VBlankIntrWait();
		scanKeys();
		int down=keysDown();
		//if(down & KEY_B)
		//{
			//*((u16*)0x4000002) ^=1;			
		//}		
		VBlankIntrWait();
		bg3.set_scroll(x[t],y[t]);
		t++;
		if(t==300) t=0;
		bg0.build_map();
		bg3.build_map();		
		
		bg0.key_down(down);
		bg0.render();
		OamPool::deploy();
	}
}


