#include "level.hpp"
#include "notebook-sheet.h"
#include "test_level.h"

#include <string.h>

class LevelBackgroundBage : public Background
{
public:
	LevelBackgroundBage() : Background(3,2,31,75,105) { strcpy(magic,"LevelBgPage"); }
	
	void init() override
	{
		Background::load_tiles(notebook_sheetTiles,notebook_sheetTilesLen, true, 0);
		Background::set_map_stream_source(notebook_sheetMap);
		dmaCopy((u8*)notebook_sheetPal,(u8*)(BG_PALETTE),notebook_sheetPalLen);
	}	
};

class LevelDungeon : public Background
{
public:
	LevelDungeon() : Background(2,0,29,75,105) { strcpy(magic,"LevelDungeon"); }
	
	void init() override
	{		
		Background::load_tiles(test_levelTiles,test_levelTilesLen, true, 160);
		Background::set_map_stream_source(test_levelMap);			
		dmaCopy((u8*)test_levelPal,(u16*)(BG_PALETTE)+160, test_levelPalLen);
	}	
};

void Level::init() 
{		
	LevelBackgroundBage* bg_page = new LevelBackgroundBage();
	set_background(3, bg_page, 0x10);
	
	LevelDungeon* dungeon = new LevelDungeon();
	set_background(2, dungeon, 0x10);
	
	REG_BLDCNT = (1<<2) | (1<<6) | (1<<11);
	REG_BLDALPHA = 13 | (3<<8);
	
	player = new Sprite(SIZE_16x16,1);
	register_sprite(player);
	camera->follow(player);
}

void Level::on_key_held(int keys)
{		
	if(keys & KEY_UP) player->move(0,-3);
	if(keys & KEY_DOWN)  player->move(0,3);
	
	if(keys & KEY_LEFT) player->move(-3,0);
	if(keys & KEY_RIGHT) player->move(3,0);
}
