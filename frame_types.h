#pragma once
#include "types.h"
#pragma pack(1)
struct same_frame 
{ 
	u1 frame_type; /* 0-63 */ 
};

struct same_locals_1_stack_item_frame
{
	u1 frame_type; /* 64-127 */
	verification_type_info stack[1];
};

struct same_locals_1_stack_item_frame_extended
{
	u1 frame_type; /* 247 */
	u2 offset_delta;
	verification_type_info stack[1];
};

struct chop_frame 
{
	u1 frame_type; /* 248-250 */
	u2 offset_delta;
};

struct same_frame_extended 
{
	u1 frame_type; /* 251 */
	u2 offset_delta;
};

struct append_frame 
{
	u1 frame_type; /* 252-254 */
	u2 offset_delta;
	verification_type_info** locals; /*[frame_type - 251]*/
};

struct full_frame 
{
	u1 frame_type; /* 255 */
	u2 offset_delta;
	u2 number_of_locals;
	verification_type_info** locals;/* [number_of_locals] ;*/
	u2 number_of_stack_items;
	verification_type_info** stack;/* [number_of_stack_items] ;*/
};

union stack_map_frame 
{
	same_frame sameframe;
	same_locals_1_stack_item_frame sl1sif;
	same_locals_1_stack_item_frame_extended sl1sife;
	chop_frame chopframe;
	same_frame_extended sfe;
	append_frame appendframe;
	full_frame fullframe;
};
#pragma pack()