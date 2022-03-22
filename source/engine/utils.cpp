#include "utils.hpp"

sf24::sf24() {}

sf24::sf24(s16 x) : sf24(x,0) {	}

sf24::sf24(s16 x, u8 frac)
{
	this->data = (x<<16)|(frac<<8);
}

s16 sf24::get_int() const
{
	return data>>16;
}

u8 sf24::get_frac() const
{
	return ((data>=0 ? data: -data) & 0x0000FF00)>>8;
}

sf24 sf24::operator + (const sf24& b) const
{	
	return create_from_data(data+b.data);
}

sf24 sf24::operator - (const sf24& b) const
{	
	return create_from_data(data-b.data);
}

sf24 sf24::operator * (const sf24& b) const
{	
	u32 sgn = (data ^ b.data) & 0x80000000;
	u32 d1 = data, d2=b.data;
	d1 = d1>=0 ? d1: -d1;
	d2 = d2>=0 ? d2: -d2;
	
	u32 res = d1 * (d2>>16); // d1* int(d2)
	d2 = (d2 & 0x0000FF00)>>8; // frac(d2)	
	res+= (data * d2)>>8;
	res &= 0x7FFFFFFF;
	res |= sgn;	
	return create_from_data(res);
}

void sf24::operator += (const sf24& b)
{
	data+=b.data;	
}

void sf24::operator -= (const sf24& b)
{
	data-=b.data;
}

void sf24::operator *= (const sf24& b)
{
	u32 sgn = (data ^ b.data) & 0x80000000;
	u32 d1 = data, d2=b.data;
	d1 = d1>=0 ? d1: -d1;
	d2 = d2>=0 ? d2: -d2;
	
	u32 res = d1 * (d2>>16); // d1* int(d2)
	d2 = (d2 & 0x0000FF00)>>8; // frac(d2)	
	res+= (data * d2)>>8;
	res &= 0x7FFFFFFF;
	res |= sgn;	
	data=res;
}

sf24::operator int() const
{
	return get_int();
}

sf24::operator s16() const
{
	return get_int();
}

sf24 sf24::create_from_data(u32 data)
{
	sf24 result;	
	result.data=data;
	return result;
}














