#pragma once

#include <gba.h>
#include "utils.hpp"

class Camera 
{
private:
	u16 x, y;
public:
	Camera();
	Camera(u16 x, u16 y);
	u16 get_x() const;
	u16 get_y() const;
	
	void set_x(u16 value);	
	void set_y(u16 value);	
};