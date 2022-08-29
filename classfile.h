#pragma once
#include "cp_info.h"
#include "attribute_info.h"
#include "filed_info.h"
#include "method_info.h"

#pragma pack(1)
struct ClassFile {
    u4             magic;
    u2             minor_version;
    u2             major_version;
    u2             constant_pool_count;
    struct cp_info**constant_pool; // cp_info constant_pool[constant_pool_count-1]
    u2             access_flags;
    u2             this_class;
    u2             super_class;
    u2             interfaces_count;
    u2*           interfaces;// u2 interfaces[interfaces_count]
    u2             fields_count;
    struct field_info**     fields;// field_info fileds[fields_count]
    u2             methods_count;
    struct method_info**    methods;// method_info methods[methods_count]
    u2             attributes_count;
    struct attribute_info** attributes;// attribute_info attributes[attributes_count]
};
#pragma pack()
