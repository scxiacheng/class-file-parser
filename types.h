#pragma once
#include <stdint.h>

typedef uint8_t             u1;
typedef uint16_t            u2;
typedef uint32_t            u4;
typedef uint64_t            u8;
typedef int8_t              s1;
typedef int16_t             s2;
typedef int32_t             s4;
typedef int64_t             s8;

enum Constant_Type
{
    CONSTANT_Class = 7,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_String = 8,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_NameAndType = 12,
    CONSTANT_Utf8 = 1,
    CONSTANT_MethodHandle = 15,
    CONSTANT_MethodType = 16,
    CONSTANT_InvokeDynamic = 18
};
#pragma pack(1)
struct exception_table
{
    u2 start_pc;
    u2 end_pc;
    u2 handler_pc;
    u2 catch_type;
};

struct Top_variable_info
{
    u1 tag; /* 0 */
};
struct Integer_variable_info
{
    u1 tag; /* 1 */
};
struct Float_variable_info
{
    u1 tag; /* 2 */
};
struct Long_variable_info
{
    u1 tag; /* 4 */
};
struct Double_variable_info
{
    u1 tag; /* 3 */
};
struct Null_variable_info
{
    u1 tag; /* 5 */
};
struct UninitializedThis_variable_info
{
    u1 tag; /* 6 */
};
struct Object_variable_info
{
    u1 tag; /* 7 */
    u2 cpool_index;
};
struct Uninitialized_variable_info
{
    u1 tag; /* 8 */
    u2 offset;
};

union verification_type_info {
    Top_variable_info top_var_info;
    Integer_variable_info int_var_info;
    Float_variable_info float_var_info;
    Long_variable_info long_var_info;
    Double_variable_info double_var_info;
    Null_variable_info null_var_info;
    UninitializedThis_variable_info uninitedthis_var_info;
    Object_variable_info obj_var_info;
    Uninitialized_variable_info uninit_var_info;
};


struct inner_class
{
    u2 inner_class_info_index;
    u2 outer_class_info_index;
    u2 inner_name_index;
    u2 inner_class_access_flags;
};

struct line_number_table
{
    u2 start_pc;
    u2 line_number;
};

struct local_variable_table
{
    u2 start_pc;
    u2 length;
    u2 name_index;
    u2 descriptor_index;
    u2 index;
};

struct local_variable_type_table
{
    u2 start_pc;
    u2 length;
    u2 name_index;
    u2 signature_index;
    u2 index;
};

struct element_value_pairs;
struct element_value;

struct annotation {
    u2 type_index;
    u2 num_element_value_pairs;
    element_value_pairs** pairs;
};

struct enum_const_value
{
    u2 type_name_index;
    u2 const_name_index;
};
struct element_value;
struct array_value
{
    u2 num_values;
    struct element_value** values;
};

struct element_value 
{
    u1 tag;
    union 
    {
        u2 const_value_index;
        enum_const_value e_const_value;
        u2 class_info_index;
        annotation *annotation_value;
        array_value *arrayvalue;
    } value;
};

struct element_value_pairs
{
    u2 element_name_index;
    struct element_value *value;
};

struct parameter_annotation
{
    u2         num_annotations;
    annotation** annotations/*[num_annotations]*/;
};

struct bootstrap_method
{
    u2 bootstrap_method_ref;
    u2 num_bootstrap_arguments;
    u2 *bootstrap_arguments/*[num_bootstrap_arguments]*/;
};
#pragma pack()

struct Pointer
{
    char t;//'o'-simple object,'['-normal array, '{'-Object array
    int n; // if p is an pointer array,then release array first 
    void* p;
    Pointer(char t, int n, void* p)
    {
        this->t = t;
        this->n = n;
        this->p = p;
    }
};