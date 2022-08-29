#pragma once
#include "types.h"

#pragma pack(1)
struct cp_info
{
    u1 tag;
};
struct CONSTANT_Class_info : cp_info
{
    u2 name_index;
};

struct CONSTANT_Fieldref_info : cp_info
{
    u2 class_index;
    u2 name_and_type_index;
};

struct CONSTANT_Methodref_info : cp_info
{
    u2 class_index;
    u2 name_and_type_index;
};

struct CONSTANT_InterfaceMethodref_info : cp_info
{
    u2 class_index;
    u2 name_and_type_index;
};

struct CONSTANT_String_info : cp_info
{
    u2 string_index;
};

struct CONSTANT_Integer_info : cp_info
{
    u4 bytes;
};

struct CONSTANT_Float_info : cp_info
{
    u4 bytes;
};

struct CONSTANT_Long_info : cp_info
{
    u4 high_bytes;
    u4 low_bytes;
};

struct CONSTANT_Double_info : cp_info
{
    u4 high_bytes;
    u4 low_bytes;
};

struct CONSTANT_NameAndType_info : cp_info
{
    u2 name_index;
    u2 descriptor_index;
};

struct CONSTANT_Utf8_info : cp_info
{
    u2 length;
    u1* bytes;
};

struct CONSTANT_MethodHandle_info : cp_info
{
    u1 reference_kind;
    u2 reference_index;
};

struct CONSTANT_MethodType_info : cp_info
{
    u2 descriptor_index;
};

struct CONSTANT_InvokeDynamic_info : cp_info
{
    u2 bootstrap_method_attr_index;
    u2 name_and_type_index;
};
#pragma pack()