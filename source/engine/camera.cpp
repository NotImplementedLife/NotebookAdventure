#include "camera.hpp"

#include "base.hpp"
#include "sprite.hpp"

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

s16 Camera::get_x() const { return target ? (s16)(target->pos_x) : (s16)x; }

s16 Camera::get_y() const { return target ? (s16)(target->pos_y) : (s16)y; }

void Camera::set_x(const sf24& value) { x = value; target=NULL; }

void Camera::set_y(const sf24& value) { y = value; target=NULL; }

void Camera::move(sf24 x, sf24 y)
{
	this->x += x;
	this->y += y;
	target=NULL;
}

#include "error.hpp"

void Camera::follow(Sprite* spr)
{
	target=spr;	
}