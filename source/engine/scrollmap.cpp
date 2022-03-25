#include "engine/scrollmap.hpp"

TextScrollMap::TextScrollMap()
{
	OamPool::reset();
	for(int i=0;i<4;i++) 
	{
		bg[i] = NULL;
		scroll_speed[i] = 0x10;
	}
	camera = new Camera();
	
	sprites_cap = 1;
	sprites = new Sprite*[sprites_cap];
	sprites_count=0;
}

void TextScrollMap::init() { }

void TextScrollMap::set_background(u8 i, Background* bgi, u8 scroll_speedi)
{
	bg[i]=bgi;
	scroll_speed[i]=scroll_speedi;
}

void TextScrollMap::on_frame() { }

void TextScrollMap::on_key_down(int keys) { }
void TextScrollMap::on_key_held(int keys) { }
void TextScrollMap::on_key_up(int keys) { } 

#include "error.hpp"

void TextScrollMap::run()
{	
	for(int i=0;i<4;i++)
		if(bg[i])
		{			
			bg[i]->init();	
		}	
	while(1)
	{
		VBlankIntrWait();
		scanKeys();
		int keys_down = keysDown();
		int keys_held = keysHeld();
		int keys_up = keysUp();		

		on_key_down(keys_down);
		on_key_held(keys_held);
		on_key_up(keys_up);
		
		on_frame();	
		for(int i=0;i<sprites_count;i++)
			sprites[i]->update_position(camera);
						
		for(int i=0;i<4;i++)		
			if(bg[i])
			{						
				bg[i]->set_scroll((camera->get_x() * scroll_speed[i])>>4, (camera->get_y() * scroll_speed[i])>>4);
				bg[i]->build_map();
				bg[i]->key_down(keys_down);
				bg[i]->key_held(keys_held);
				bg[i]->key_up(keys_up);
				bg[i]->render();
			}		
			
		OamPool::deploy();
	}
}

Camera* TextScrollMap::get_camera() const
{ 
	return camera;
}

void TextScrollMap::register_sprite(Sprite* spr)
{
	if(sprites_count==sprites_cap)
	{
		sprites_cap *= 2;
		Sprite** container = new Sprite*[sprites_cap];
		for(int i=0;i<sprites_count;i++)
			container[i] = sprites[i];		
		delete[] sprites;
		sprites = container;
	}
	sprites[sprites_count++]=spr;
	
}

TextScrollMap::~TextScrollMap()
{
	for(int i=0;i<4;i++)
		if(bg[i])
			delete bg[i];
	delete camera;
	OamPool::reset();
}