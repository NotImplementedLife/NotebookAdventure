#include <gba.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine.hpp"

#include "game_dat.hpp"
#include "level.hpp"
#include "title_screen.hpp"
#include "audio.hpp"


#include "level_1_bin.h"
#include "level_2_bin.h"
#include "all_levels.h"


int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);
	SetMode(MODE_0 | OBJ_ENABLE | OBJ_1D_MAP);
		
	init_sound();
	//while(1) VBlankIntrWait();
		
	OamPool::reset();
	
	load_user_data();	
	
	int screen_result = LVL_MENU;
	TextScrollMap* screen = NULL;	
		
	u16 last_enter_id=0;
	
	/*u8* x=new u8[0x10000];
	x[0]=LVL_MENU;
	screen_result=x[0];
	delete[] x;*/	
	
	while(1)
	{		
		switch(screen_result&3)
		{
			case LVL_MENU:
			{								
				save_user_data();				
				screen = new TitleScreen();
				break;
			}
			case LVL_ENTER:
			{
				last_enter_id = screen_result>>2;
				screen = new Level(last_enter_id);						
				break;
			}
			case LVL_NEXT:
			{
				last_enter_id++;					
				screen = new Level((u32)last_enter_id);
				break;
			}
		}			
		screen->init();
		screen_result = screen->run();		
		delete screen;			
		if(last_enter_id+1>UserData.current_level)
			UserData.current_level = last_enter_id+1;
		save_user_data();
	}
	return 0;
}


