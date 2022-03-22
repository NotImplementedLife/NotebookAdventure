#include "camera.hpp"

Camera::Camera()
{
	x = y = 0;
}

Camera::Camera(u16 x, u16 y)
{
	this->x = x;
	this->y = y;
};

u16 Camera::get_x() const { return x; }

u16 Camera::get_y() const { return y; }

void Camera::set_x(u16 value) { x = value; }

void Camera::set_y(u16 value) { y = value; }