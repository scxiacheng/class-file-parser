#pragma once
#include "attribute_info.h"
#pragma pack(1)
struct field_info
{
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    attribute_info** attributes;/* [attributes_count] ;*/
};
#pragma pack()