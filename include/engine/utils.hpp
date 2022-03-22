#pragma once

#include <gba.h>

/*! \brief signed fixed 16.8 decimal number 
 */
class sf24
{
private:
	u32 data;
	sf24(u32 data);
public:
	sf24(s16 x);		
	sf24(s16 x,u8 frac);
	
	s16 get_int() const;
	u8 get_frac() const;
	
	sf24 operator + (const sf24& b) const;
	sf24 operator - (const sf24& b) const;
	sf24 operator * (const sf24& b) const;
	
	u32 to_u32() const;
};

