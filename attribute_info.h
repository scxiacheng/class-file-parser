#pragma once
#include "types.h"
#include "frame_types.h"
#pragma pack(1)
struct attribute_info
{
	u2 attribute_name_index;
	u4 attribute_length;
};

struct ConstantValue_attribute : attribute_info
{
    u2 constantvalue_index;
};

struct Code_attribute : attribute_info
{
    u2 attribute_name_index;
    u4 attribute_length;
    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    u1* code;
    u2 exception_table_length;
    exception_table** exc_tables;
    u2 attributes_count;
    attribute_info** attributes;
};

struct StackMapTable_attribute : attribute_info
{
    u2 number_of_entries;
    stack_map_frame** entries;//entries[number_of_entries]
};

struct Exceptions_attribute : attribute_info
{
    u2 number_of_exceptions;
    u2* exception_index_table;/* [number_of_exceptions] ;*/
};

struct InnerClasses_attribute: attribute_info
{
    u2 number_of_classes;
    inner_class** classes;//[number_of_classes]
};

struct EnclosingMethod_attribute : attribute_info
{
    u2 class_index;
    u2 method_index;
};

struct Synthetic_attribute : attribute_info
{
};

struct Signature_attribute : attribute_info
{
    u2 signature_index;
};

struct SourceFile_attribute : attribute_info
{
    u2 sourcefile_index;
};

struct SourceDebugExtension_attribute : attribute_info
{
    u1* debug_extension;/* [attribute_length] ;*/
};

struct LineNumberTable_attribute : attribute_info
{
    u2 line_number_table_length;
    line_number_table** line_number_tables; /*[line_number_table_length]*/
};

struct LocalVariableTable_attribute : attribute_info 
{
    u2 local_variable_table_length;
    local_variable_table** tables; /*[local_variable_table_length]*/
};

struct LocalVariableTypeTable_attribute : attribute_info
{
    u2 local_variable_type_table_length;
    local_variable_type_table** tables;/* [local_variable_type_table_length] ;*/
};

struct Deprecated_attribute : attribute_info
{

};

struct RuntimeVisibleAnnotations_attribute :attribute_info
{
    u2 num_annotations;
    annotation** annotations;/* [num_annotations] ;*/
};

struct RuntimeInvisibleAnnotations_attribute :attribute_info 
{
    u2         num_annotations;
    annotation **annotations/*[num_annotations]*/;
};

struct RuntimeVisibleParameterAnnotations_attribute : attribute_info
{
    u1 num_parameters;
    parameter_annotation** parameter_annotations/*[num_parameters]*/;
};

struct RuntimeInvisibleParameterAnnotations_attribute : RuntimeVisibleParameterAnnotations_attribute
{

};

struct AnnotationDefault_attribute :attribute_info
{
    element_value *default_value;
};

struct BootstrapMethods_attribute : attribute_info
{
    u2 num_bootstrap_methods;
    bootstrap_method** bootstrap_methods;/*[num_bootstrap_methods]*/
};
#pragma pack()