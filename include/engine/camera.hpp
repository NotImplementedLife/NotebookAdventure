#pragma once

#include <gba.h>
#include "utils.hpp"

class Sprite;

class Camera 
{
	friend class Sprite;
private:
	sf24 x, y;
	Sprite* target;
public:
	Camera();
	Camera(sf24 x, sf24 y);
	s16 get_x() const;
	s16 get_y() const;
	
	void set_x(const sf24& value);	
	void set_y(const sf24& value);
	
	void move(sf24 x, sf24 y);
	
	void follow(Sprite* spr);
	
};