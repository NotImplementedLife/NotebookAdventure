#include "camera.hpp"

#include "sprite.hpp"
#include "debug.hpp"

Camera::Camera()
{
	target=NULL;
	x = y = 0;
}

Camera::Camera(sf24 x, sf24 y)
{
	this->x = x;
	this->y = y;
};

s16 Camera::get_x() const 
{
	if(!target) return (s16)x;
	
	s16 _x = (s16)(target->pos_x);
	
	if(_x<120) return 120;	
	if(_x>bndw) return bndw;
	
	return _x; 
}

s16 Camera::get_y() const 
{
	if(!target) return (s16)y;
	
	s16 _y = (s16)(target->pos_y);
	
	if(_y<80) return 80;	
	if(_y>bndh) return bndh;
	
	return _y; 
}

void Camera::set_x(const sf24& value) { x = value; target=NULL; }

void Camera::set_y(const sf24& value) { y = value; target=NULL; }

void Camera::move(sf24 x, sf24 y)
{
	this->x += x;
	this->y += y;
	target=NULL;
}

void Camera::set_bounds(s16 w, s16 h)
{
	bndw = w-120-1;
	bndh = h-80-1;
}

#include "error.hpp"

void Camera::follow(Sprite* spr)
{
	target=spr;	
}