#include "camera.hpp"

Camera::Camera()
{
	x = y = 0;
}

Camera::Camera(sf24 x, sf24 y)
{
	this->x = x;
	this->y = y;
};

s16 Camera::get_x() const { return (s16)x; }

s16 Camera::get_y() const { return (s16)y; }

void Camera::set_x(const sf24& value) { x = value; }

void Camera::set_y(const sf24& value) { y = value; }

void Camera::move(sf24 x, sf24 y)
{
	this->x += x;
	this->y += y;
}