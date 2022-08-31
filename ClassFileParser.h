#pragma once
#include "classfile.h"
#include <fstream>
#include <string>
#include <vector>

class CClassFileParser
{
public:
	CClassFileParser(const std::string& class_file);
	~CClassFileParser();
	ClassFile* Parse();
	void Release();
private:
	int getclassfilesize();
	void read_magic();
	void read_minorversion();
	void read_majorversion();
	void read_constpoolsize();
	void read_constpools();
	void read_accessflag();
	void read_thisclass();
	void read_supperclass();
	void read_interfacecount();
	void read_interfaces();
	void read_fieldscount();
	void read_fields();
	void read_methodscount();
	void read_methods();
	void read_attributescount();
	void read_attributes();
	void read_constant_class(cp_info** pinfo);
	void read_constant_fieldref(cp_info** pinfo);
	void read_constant_methodref(cp_info** pinfo);
	void read_constant_interfacemethodref(cp_info** pinfo);
	void read_constant_string(cp_info** pinfo);
	void read_constant_integer(cp_info** pinfo);
	void read_constant_float(cp_info** pinfo);
	void read_constant_long(cp_info** pinfo);
	void read_constant_double(cp_info** pinfo);
	void read_constant_nameandtype(cp_info** pinfo);
	void read_constant_utf8(cp_info** pinfo);
	void read_constant_methodhandle(cp_info** pinfo);
	void read_constant_methodtype(cp_info** pinfo);
	void read_constant_invokedynamic(cp_info** pinfo);
	inline u2 swapu2(u2 x);
	inline u4 swapu4(u4 x);
	void read_fieldsinfo(field_info** pinfo);
	void read_field_attributeinfo(attribute_info** pinfo);
	char* get_attribute_name(u2 name_index);
	void read_ConstantValue_attrinfo(attribute_info** pinfo);
	void read_Synthetic_attrinfo(attribute_info** pinfo);
	void read_Signature_attrinfo(attribute_info** pinfo);
	void read_Deprecated_attrinfo(attribute_info** pinfo);
	void read_RuntimeVisibleAnnotations_attrinfo(attribute_info** pinfo);
	void read_RuntimeInvisibleAnnotations_attrinfo(attribute_info** pinfo);
	void read_annotation(annotation** pinfo);
	void read_element_value_pair(element_value_pairs** pair);
	void read_arrayvalue(array_value** value);
	void read_elementvalue(element_value** value);
	void read_methodsinfo(method_info** pinfo);
	void read_method_attributeinfo(attribute_info** pinfo);
	void read_code_attribute(attribute_info** pinfo);
	void read_exception_attribute(attribute_info** pinfo);
	void read_RuntimeVisibleParameterAnnotations_attrinfo(attribute_info** pinfo);
	void read_RuntimeInvisibleParameterAnnotations_attrinfo(attribute_info** pinfo);
	void read_AnnotationDefault_attrinfo(attribute_info** pinfo);
	void read_exception_table(exception_table** ptable);
	void read_code_attributeinfo(attribute_info** pinfo);
	void read_LineNumberTable_attribute(attribute_info** pinfo);
	void read_LocalVariableTable_attribute(attribute_info** pinfo);
	void read_LocalVariableTypeTable_attribute(attribute_info** pinfo);
	void read_StackMapTable_attribute(attribute_info** pinfo);
	void read_line_number_table(line_number_table** ptable);
	void read_local_variable_table(local_variable_table** ptable);
	void read_local_variable_type_table(local_variable_type_table** ptable);
	void read_stack_map_frame(stack_map_frame** frame);
	void read_parameter_annotation(parameter_annotation** ppa);
	void read_attributes_attributeinfo(attribute_info** pinfo);
	void read_InnerClasses_attribute(attribute_info** pinfo);
	void read_EnclosingMethod_attribute(attribute_info** pinfo);
	void read_SourceFile_attrinfo(attribute_info** pinfo);
	void read_SourceDebugExtension_attrinfo(attribute_info** pinfo);
	void read_BootstrapMethods_attrinfo(attribute_info** pinfo);
	void read_innerclass(inner_class** ic);
	void read_bootstrap_method(bootstrap_method** bm);
	void read_verification_type_info(verification_type_info** pinfo);
	void delete_array(void** array, int n);
	void move_position(int offset);
private:
	std::string input_class_file;
	u1* file_data;
	u1* where_;
	ClassFile* pClassFile;
	std::vector<Pointer*> m_Ptrs;
};