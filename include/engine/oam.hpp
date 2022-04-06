/*! \file oam.hpp
	\brief OAM-related definitions
 */
#pragma once

#include <gba.h>

/*! \enum ObjSize
	\brief all possible GBA sprite sizes
 */
enum ObjSize 
{
	SIZE_8x8   =  0, //!< Sprite size 8x8  
	SIZE_16x16 =  1, //!< Sprite size 16x16 
	SIZE_32x32 =  2, //!< Sprite size 32x32 
	SIZE_64x64 =  3, //!< Sprite size 64x64 
	SIZE_16x8  =  4, //!< Sprite size 16x8  
	SIZE_32x8  =  5, //!< Sprite size 32x8  
	SIZE_32x16 =  6, //!< Sprite size 32x16 
	SIZE_64x32 =  7, //!< Sprite size 64x32 
	SIZE_8x16  =  8, //!< Sprite size 8x16  
	SIZE_8x32  =  9, //!< Sprite size 8x32  
	SIZE_16x32 = 10, //!< Sprite size 16x32 
	SIZE_32x64 = 11  //!< Sprite size 32x64 
};

/*! \class ObjAttribute
	\brief describes OAM object attributes, works only with 256-color sprites	
 */
class ObjAttribute
{
private:
	u16 attr0, attr1, attr2, attr3;
public:
	ObjAttribute();
	
	/*! \fn ObjAttribute
		\brief creates an object attribute
		\param size       object size
		\param x          object position - horizontal coordinate
		\param y          object position - vertical coordinate
		\param tile_index tile number in OAM Tiles section of VRAM
		\param hflip      horizontal flip (0 = no ; non-0 = yes)
		\param hflip      vertical flip (0 = no ; non-0 = yes)
		\param priority   object priority (0..3)
	 */ 
	ObjAttribute(ObjSize size, s16 x, s16 y, u16 tile_index, u16 hflip=0, u16 vflip=0, u16 priority=0);
	
	/*! \fn set_size
		\brief set object size
		\param size object size
	 */
	void set_size(ObjSize size);
	
	/*! \brief get object size		
		\return object size 
	 */
	ObjSize get_size() const;
	
	/*! \fn flip_v
		\brief flip object vertically 	
	 */
	void flip_v();
	
	/*! \fn set_flip_v
		\brief set object vertical flip
		\param flip 0 if no flip, non-0 value if flipped
	 */
	void set_flip_v(u16 flip);
	
	/*! \fn get_flip_v
		\brief get object vertical flip
		\return 0 if no flip, 1 if flipped
	 */
	u16 get_flip_v() const;
	
	/*! \fn flip_h
		\brief flip sprite horizontally
	 */
	void flip_h();
	
	/*! \fn set_flip_h
		\brief set sprite horizontal flip
		\param flip 0 if no flip, non-0 value if flipped
	 */
	void set_flip_h(u16 flip);
	
	/*! \fn get_flip_h
		\brief get sprite horizontal flip
		\return 0 if no flip, 1 if flipped
	 */
	u16 get_flip_h() const;
	
	/*! \fn set_x
		\brief set sprite X coordinate
		\param x on-screen sprite X-coordinate (9 bits)
	 */
	void set_x(s16 x);
	
	/*! \fn set_y
		\brief set sprite Y coordinate
		\param y on-screen sprite Y-coordinate (8 bits)
	 */
	void set_y(s16 y);
	
	/*! \fn get_x
		\brief get sprite X coordinate
		\return on-screen sprite X coordinate
	 */
	s16 get_x() const;
	
	/*! \fn get_y
		\brief get sprite Y coordinate
		\return on-screen sprite Y coordinate
	 */
	s16 get_y() const;
	
	/*! \fn set_priority
		\brief set sprite priority
		\param priority sprite priority (0..3)
	 */
	void set_priority(u16 priority);
	
	/*! \fn set_tile_index
		\brief set index of the first sprite tile 
		\param tile_index sprite tile index
	 */
	void set_tile_index(u16 tile_index);
	
	u16 get_tile_index();
	
	void show();
	
	void hide();
	
	bool is_hidden() const;
};

/*! \namespace OamPool 
    \brief intermediate API handling OAM attributes
 */
namespace OamPool
{
	
	/*! \fn add_obj(ObjAttribute)
		\brief creates new object in OAM pool from object attribute
		\param objattr object attribute of the new sprite
		\return a pool object id of the created sprite
	 */
	u16 add_obj(ObjAttribute objattr);
	
	/*! \fn get_object_by_id(u16)
		\brief gets attributes of OAM pool object
		\param id the object's id
		\return pointer to the object's attribute instance		
	 */
	ObjAttribute* get_object_by_id(u16 id);	
		
	/*! \fn remove_obj(u16)
		\brief removes object from OAM pool
		\param id the object's id
	 */
	void remove_obj(u16 id);
	
	/*! \fn reset
		\brief resets the shadow OAM components
	 */
	void reset();
	
	void init();
	
	/*! \fn deploy
		\brief updates the on screen sprites state
	 */
	void deploy();	
	
};
