#include "physical_object.hpp"

static const sf24 g = sf24(2,1);

PhysicalObject::PhysicalObject(ObjSize size, u16 frames_count) : Sprite(size, frames_count) 
{
	vx = 0;
	vy = 0;	
	
	fx = sf24(2,0);
	fy = sf24(3,0);
	
	air_ymin=0;
	air_ymax=840;
	
	ax = 0;
	ay = 0;
}

void PhysicalObject::set_air_limits(s16 ymin, s16 ymax)
{
	use_air_limits=true;
	air_ymin = ymin;
	air_ymax = ymax;
}

void PhysicalObject::ignore_air_limits()
{
	use_air_limits=false;
}

void PhysicalObject::render()
{
	vx+=ax;
	vy+=ay;
	pos_x+=vx;
	pos_y+=vy;
	if(use_air_limits)
	{
		if(get_top_coord()<air_ymin) 
		{
			pos_y = air_ymin + anchy;
			if(ay<0) ay+=sf24(0,8);
		}
		else if(get_bottom_coord()>=air_ymax)
		{
			pos_y = air_ymax - hitbox.height + anchy + 1;		
			ay=0;
		}
	}
	
	if(ax>0) ax-=sf24(0, 8);
	else if(ax<0) ax+=sf24(0, 8);
	
	if(ay>0) ay-=sf24(0, 8);
	else if(ay<0) ay+=sf24(0,8);
	
	if(vx>0)
	{
		vx-=fx;
		if(vx<0) vx=0;
	}
	else if(vx<0)
	{
		vx+=fx;
		if(vx>0) vx=0;
	}
	
	if(vy>0)
	{
		vy-=fy;
		if(vy<0) vy=0;
	}		
	else if(vy<0)
	{
		vy+=fy;
		if(vy>0) vy=0;
	}
}

void PhysicalObject::charge_v(sf24 vx, sf24 vy)
{
	this->vx+=vx;
	this->vy+=vy;
}

void PhysicalObject::charge_a(sf24 ax, sf24 ay)
{
	this->ax=ax;
	this->ay=ay;
}
	

void PhysicalObject::apply_g()
{
	this->vy+=g;
}		