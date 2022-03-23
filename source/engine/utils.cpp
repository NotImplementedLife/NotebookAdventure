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

bool sf24::operator < (const sf24& b) const
{
	return (s32)data < (s32)b.data;
}

bool sf24::operator > (const sf24& b) const
{
	return (s32)data > (s32)b.data;
}

bool sf24::operator <= (const sf24& b) const
{
	return (s32)data <= (s32)b.data;
}

bool sf24::operator >= (const sf24& b) const
{
	return (s32)data >= (s32)b.data;
}

bool sf24::operator == (const sf24& b) const
{
	return data == b.data;
}

bool sf24::operator != (const sf24& b) const
{
	return data != b.data;
}

sf24 sf24::create_from_data(u32 data)
{
	sf24 result;	
	result.data=data;
	return result;
}

bool sf24::in_range(const sf24& min, const sf24& max) const
{
	return (s32)min.data<=(s32)data && (s32)data<=(s32)max.data;
}

char* sf24::to_string()
{
	s16 i = get_int();
	int sgn=0;
	u32 f = get_frac();
	
	if(i<0) 
	{
		i=-i;
		sgn=1;
		f=-f;
	}
	
	f=10000+f*10000/256;
	
	
	char si[8];
	char sf[5];
	u8 li=0, lf=0;
	
	if(i==0) 
		si[0]='0', li=1;
	for(;i>0;i/=10)	si[li++]='0'+i%10;
	if(sgn==1)
		si[li++]='-';
	
	if(f==0) 
		sf[0]='0', lf=1;
	for(;f>1;f/=10)	sf[lf++]='0'+f%10;
	
	char* result = new char[li+lf+2];
	u8 k=0;
	for(;li--;) result[k++]=si[li];
	result[k++]='.';
	for(;lf--;) result[k++]=sf[lf];
	result[k]='\0';
	return result;
}












