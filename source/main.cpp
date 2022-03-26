#include <gba.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine.hpp"

#include "level.hpp"

#include "test_level_bin.h"

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);
	SetMode(MODE_0 | OBJ_ENABLE | OBJ_1D_MAP);
		
	OamPool::reset();	
	
	Level* lvl = new Level(test_level_bin);
	lvl->init();	
	lvl->run();
	delete lvl;
	
	
}


