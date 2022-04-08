#include <gba.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine.hpp"

#include "debug.hpp"

#include "game_dat.hpp"
#include "level.hpp"
#include "title_screen.hpp"
#include "audio.hpp"


#include "level_1_bin.h"
#include "level_2_bin.h"
#include "all_levels.h"

#include "esodev_logo.hpp"

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);
	SetMode(MODE_0 | OBJ_ENABLE | OBJ_1D_MAP);
		
	init_sound();
	//while(1) VBlankIntrWait();
		
		
	OamPool::init();
	
	load_user_data();	
	
	int screen_result = LVL_MENU;
	TextScrollMap* screen = NULL;	
		
	#ifdef ESODEV
	screen = new EsoDevLogo();
	screen->init();
	screen->run();
	delete screen;
	#endif

	//dbg_ctx="Level";
	/*screen = new Level(4);
	screen->init();
	screen->run();
	//dbg_ctx="TitleScreen";
	delete screen;	*/
		
	u16 last_enter_id=0;
	
	// quick level playthrough test
	/*int lvl=1;
	while(1)
	{
		//dbg_ctx="TitleScreen";
		screen = new TitleScreen();
		screen->exit_flag=1;
		screen->init();		
		screen->run();
		//dbg_ctx="TitleScreen";
		delete screen;
		
		//dbg_ctx="Level";
		screen = new Level(lvl);
		((Level*)screen)->completed=LVL_MENU; // Quit
		screen->init();		
		screen->run();
		//dbg_ctx="TitleScreen";
		delete screen;			

		lvl++;
		if(lvl==6) lvl=1;
	}*/
	
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
		
		if(screen_result & LVL_COMPLETE)
		{
			screen_result &= ~LVL_COMPLETE;			
			if(last_enter_id+1>UserData.current_level && last_enter_id<LEVELS_COUNT)				
				UserData.current_level = last_enter_id+1;			
		}		
		save_user_data();
	}	
	
	
	return 0;
}


