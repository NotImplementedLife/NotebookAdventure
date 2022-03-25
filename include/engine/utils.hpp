#pragma once

#include <gba.h>

char* concat(char delim, ...);

/*! \brief signed fixed 16.8 decimal number 
 */
class sf24
{
private:
	u32 data;
	static sf24 create_from_data(u32 data);	
public:
	sf24();
	sf24(s16 x);
	sf24(s16 x, u8 frac);
	
	s16 get_int() const;
	u8 get_frac() const;
	
	sf24 operator + (const sf24& b) const;
	sf24 operator - (const sf24& b) const;
	sf24 operator * (const sf24& b) const;	
	
	sf24 operator -() const;
	void operator += (const sf24& b);
	void operator -= (const sf24& b);
	void operator *= (const sf24& b);	
	
	explicit operator int() const;
	explicit operator s16() const;
	
	bool operator < (const sf24& b) const;
	bool operator > (const sf24& b) const;
	bool operator <= (const sf24& b) const;
	bool operator >= (const sf24& b) const;
	bool operator == (const sf24& b) const;
	bool operator != (const sf24& b) const;
	
	bool in_range(const sf24& min, const sf24& max) const;
	
	char* to_string();
};

void reindex_palette(u16* target, u32 byteslen, u8 offset);

