#pragma once

#include "engine.hpp"

class PhysicalObject : public Sprite
{	
public:
	sf24 ax, ay;
	sf24 vx, vy;
	sf24 fx, fy;	
	sf24 air_ymin, air_ymax;
	bool use_air_limits=false;	
public:
	PhysicalObject(ObjSize size, u16 frames_count);
	void set_air_limits(s16 ymin, s16 ymax);
	void ignore_air_limits();
	void render();
	void charge_v(sf24 vx, sf24 vy);
	void charge_a(sf24 ax, sf24 ay);
	void apply_g();
};
