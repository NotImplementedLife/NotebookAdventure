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

bool TextScrollMap::input_locked() const
{
	return lock_i!=-1;
}

void TextScrollMap::lock_input(int lock_id)
{
	lock_i = lock_id;
}
void TextScrollMap::unlock_input()
{
	lock_i = -1;
}

bool TextScrollMap::execution_locked() const
{
	return lock_e!=-1;
}

void TextScrollMap::lock_execution(int lock_id)
{
	lock_e=lock_id;
	lock_input(lock_id);
}

void TextScrollMap::unlock_execution()
{
	lock_e=-1;
	unlock_input();
}

void TextScrollMap::on_loaded() { }

#include "error.hpp"

u8 TextScrollMap::run()
{	
	for(int i=0;i<4;i++)
		if(bg[i])
		{			
			bg[i]->init();	
		}	
	on_loaded();
	while(1)
	{
		VBlankIntrWait();
		if(exit_flag) 
		{
			break;
		}
		scanKeys();
		int keys_down = keysDown();
		int keys_held = keysHeld();
		int keys_up = keysUp();		

		if(lock_i==-1 || (lock_i==4))
		{
			on_key_down(keys_down);
			on_key_held(keys_held);
			on_key_up(keys_up);
		}		
				
		if(lock_e==-1)
		{			
			on_frame();
			for(int i=0;i<sprites_count;i++)
			{
				sprites[i]->update_visual();
				sprites[i]->update_position(camera);
			}
		}
						
		for(int i=0;i<4;i++)		
			if(bg[i] && (lock_e==-1 || lock_e==i))
			{						
				if(scroll_speed[i]==0)
					bg[i]->set_scroll(0,0);
				else 
					bg[i]->set_scroll(
						((camera->get_x() * scroll_speed[i])>>4)-240/2, 
						((camera->get_y() * scroll_speed[i])>>4)-160/2);
				bg[i]->build_map();
				if(lock_i==-1 || lock_i==i)
				{
					bg[i]->key_down(keys_down);
					bg[i]->key_held(keys_held);
					bg[i]->key_up(keys_up);
				}
				bg[i]->render();
			}		
			
		OamPool::deploy();
	}	
	
	return exit_flag;
}

void TextScrollMap::exit(u8 code)
{
	exit_flag=code;
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
	
	for(int i=0;i<sprites_count;i++)
	{
		delete sprites[i];
	}
	delete[] sprites;
	
	OamPool::reset();
}