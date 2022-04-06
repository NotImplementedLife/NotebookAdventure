#include "oam.hpp"

#include "error.hpp"

#include "debug.hpp"

ObjAttribute::ObjAttribute()
{
	attr1=attr2=attr3=0;
	attr0 = 1<<13; // 256 colors	
}

ObjAttribute::ObjAttribute(ObjSize size, s16 x, s16 y, u16 tile_index, u16 hflip, u16 vflip, u16 priority)
{
	attr0 = y  
			| (1<<13)              // 256 colors	
			| ((size & 0xC )<<12); // shape
	attr1 = x
			| ((!!hflip) << 12) 
			| ((!!vflip) << 13) 
			| (size<<14);          // size
	attr2 = tile_index
			| ((priority&3) << 10);	
}

void ObjAttribute::set_size(ObjSize size)
{
	// set shape
	attr0 = (attr0 & 0x3FFF) | ((size & 0xC )<<12);
	// set size	
	attr1 = (attr1 & 0x3FFF) | (size<<14);
}

ObjSize ObjAttribute::get_size() const
{
	//u8 shape = (attr0 & 0xC000)>>12;
	//u8 size  = (attr1 & 0xC000)>>12;
	//return 	(shape<<2) | size;
	return (ObjSize)(((attr0 & 0xC000) | (attr1>>2))>>12);
}

void ObjAttribute::flip_v()
{
	attr1 ^= (1<<13);
}

void ObjAttribute::set_flip_v(u16 flip)
{
	attr1 = (attr1 & (~(1<<13))) | ((!!flip)<<13);
}
		
u16 ObjAttribute::get_flip_v() const
{
	return !!(attr1&(1<<13));
}

void ObjAttribute::flip_h()
{
	attr1 ^= (1<<12);
}

void ObjAttribute::set_flip_h(u16 flip)
{
	attr1 = (attr1 & (~(1<<12))) | ((!!flip)<<12);
}
		
u16 ObjAttribute::get_flip_h() const
{
	return !!(attr1&(1<<12));
}

void ObjAttribute::set_x(s16 x)
{	
	attr1 &= ~0x01FF;
	if(x < -64) 
		x=-64;
	else if(x > 240+64) 
		x = 240+64;
	attr1 |= x & 0x01FF;
}
	
void ObjAttribute::set_y(s16 y)
{
	if(y < -64) 
		y=-64;
	else if(y > 160+64) 
		y = 160+64;
	*((s8*)(&attr0)) = (s8)y;
}
	
s16 ObjAttribute::get_x() const
{
	return OBJ_X(attr1);
}
	
s16 ObjAttribute::get_y() const
{
	return OBJ_Y(attr0);
}

void ObjAttribute::set_priority(u16 priority)
{
	attr2 &= ~(3<<10);
	attr2 |= (priority&3)<<10;
}

void ObjAttribute::set_tile_index(u16 tile_index)
{
	attr2 &= ~0x03FF;
	attr2 |= tile_index & 0x03FF;
}

u16 ObjAttribute::get_tile_index()
{
	return attr2 & 0x03FF;
}

void ObjAttribute::show()
{
	attr0 &= ~(1<<9);	
}
	
void ObjAttribute::hide()
{
	attr0 |= 1<<9;
}

bool ObjAttribute::is_hidden() const
{
	return (attr0 & (1<<9));
}


namespace OamPool
{	
	ObjAttribute __oam_buffer[128];
	u8 __oam_busy[32];	
	u16 __oam_size = 0;
	u16 __oam_id_cnt = 0;	
	
	struct frag32
	{
		u16 hw1;
		u16 hw2;
	};
	
	frag32* obj_addr = NULL;
	u16 obj_cap=0;
	u16 obj_len=0;
	
	void init()
	{
		__oam_size = __oam_id_cnt = 0;
		for(int i=0;i<32;i++) __oam_busy[i]=0;	
		obj_cap=1;
		obj_len=0;	
		dbg_ctx="obj_addr";
		obj_addr=new frag32[obj_cap];
	}	
	
	void reset()
	{
		__oam_size = __oam_id_cnt = 0;
		for(int i=0;i<32;i++) __oam_busy[i]=0;	
		
		dbg_ctx="obj_addr";
		delete[] obj_addr;
		obj_cap=obj_len=0;
		
		init();
	}	
	
	u16 get_free_id()
	{
		for(int i=0;i<32;i++)		
			for(int k=0;k<8;k++)
			{
				if(!(__oam_busy[i] & (1<<k)))
				{
					return (i<<3) | k;
				}
			}
		return 128;
	}
	
	inline void lock_id(u16 id)
	{
		__oam_busy[id>>3] |= 1 << (id & 7);
	}
	
	inline void unlock_id(u16 id)
	{
		__oam_busy[id>>3] &= ~ (1 << (id & 7));
	}
	
	u16 add_obj(ObjAttribute objattr)
	{		
		if(__oam_size==128)
		{
			fatal(ERR_OAM_FULL);
		}
		u16 id = __oam_id_cnt++;
		if(__oam_id_cnt==0)
		{
			fatal(ERR_OAM_ID_OVERFLOW);
		}		
				
		if(obj_len>=obj_cap)
		{					
			obj_cap<<=1;
			dbg_ctx="redim(obj_addr)";
			frag32* container = new frag32[obj_cap];
			for(int i=0;i<obj_len;i++)
				container[i]=obj_addr[i];
			dbg_ctx="del_old(obj_addr)";
			delete[] obj_addr;
			obj_addr=container;
			
		}
		u16 buff_id = get_free_id();
		
		obj_addr[obj_len].hw1=id;
		obj_addr[obj_len].hw2=buff_id;
		obj_len++;		
		
		//__oam_buffer[buff_id] = objattr;
		u16* dst = (u16*)(&__oam_buffer[buff_id]);
		u16* src = (u16*)(&objattr);
		dst[0]=src[0];		
		dst[1]=src[1];		
		dst[2]=src[2];
		
		lock_id(buff_id);		
		__oam_size++;
				
		DEBUG_MSG("  add %i\n",id);
		return id;
	}
	
	ObjAttribute* get_object_by_id(u16 id)
	{		
		for(u16 i=0;i<obj_len;i++)
		{
			if(obj_addr[i].hw1==id)
				return &__oam_buffer[obj_addr[i].hw2];
		}
		
		fatal(ERR_OAM_INVALID_ID,"get_object_by_id()");
		return NULL;
	}
	
	void remove_obj(u16 id)
	{		
		DEBUG_MSG("  rem %i\n",id);		
		u16 p=-1;
		for(u16 i=0;i<obj_len;i++)
		{
			if(obj_addr[i].hw1==id)
			{
				p=obj_addr[i].hw2;
				
				for(int j=i;j<obj_len-1;j++)
					obj_addr[j]=obj_addr[j+1];								
				obj_len--;
				break;
			}
		}				
		if(p==-1) 
		{
			fatal(ERR_OAM_INVALID_ID, "remove_obj()");
		}						
		unlock_id(p);
		u32* ptr = (u32*)&__oam_buffer[p];			
		ptr[0] = ptr[1] = 0x00000000;		
		__oam_size--;			
	}		
	
	void deploy()
	{		
		dmaCopy(__oam_buffer, OAM, 128 << 3);
	}		
}






