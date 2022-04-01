#include <gba.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine.hpp"

#include "game_dat.hpp"
#include "level.hpp"
#include "title_screen.hpp"


#include "level_1_bin.h"
#include "level_2_bin.h"
#include "all_levels.h"


int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);
	SetMode(MODE_0 | OBJ_ENABLE | OBJ_1D_MAP);
		
	
	//while(1) VBlankIntrWait();
		
	OamPool::reset();
	
	load_user_data();	
	
	int screen_result = LVL_MENU;
	TextScrollMap* screen = NULL;	
		
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
				screen = new Level(1);
				break;
			}
			case LVL_NEXT:
			{
				save_user_data();				
				screen = new TitleScreen();
				break;
			}
		}	
		screen->init();
		screen_result = screen->run();
		delete screen;					
	}
	TitleScreen* ts = new TitleScreen();
	ts->init();
	fatal("result",ts->run());
	delete ts;
	
	
	Level* lvl = new Level(1);
	lvl->init();	
	lvl->run();
	delete lvl;
	
	lvl = new Level(1);
	lvl->init();
	lvl->run();
	delete lvl;
	
	
	return 0;
}


