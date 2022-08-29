#include "ClassFileParser.h"

CClassFileParser::CClassFileParser(const std::string& class_file):
	input_class_file(class_file),
	file_data(NULL),
	where_(NULL),
	pClassFile(NULL)
{
}

CClassFileParser::~CClassFileParser()
{
}

ClassFile* CClassFileParser::Parser()
{
	if (0 == getclassfilesize())
	{
		return NULL;
	}
	
	pClassFile = new ClassFile;
	if (!pClassFile)
	{
		return NULL;
	}
	read_magic();
	read_minorversion();
	read_majorversion();
	read_constpoolsize();
	read_constpools();
	read_accessflag();
	read_thisclass();
	read_supperclass();
	read_interfacecount();
	read_interfaces();
	read_fieldscount();
	read_fields();
	read_methodscount();
	read_methods();
	read_attributescount();
	read_attributes();
	return pClassFile;
}

void CClassFileParser::Release()
{
	std::vector<Pointer*>::iterator it = m_Ptrs.begin();
	for (it; it != m_Ptrs.end(); it++)
	{
		switch ((*it)->t)
		{
		case 'o': {delete (*it)->p; break;}
		case '[': {delete [](*it)->p;break;}
		case '{': {delete_array(&(*it)->p, (*it)->n);break;}
		}
		delete* it;
	}

	delete[] file_data;
	delete pClassFile;
	pClassFile = NULL;
	where_ = NULL;
}

int CClassFileParser::getclassfilesize()
{
	std::ifstream is;
	is.open(input_class_file, std::ios_base::binary);
	if (!is.is_open())
	{
		return 0;
	}
	std::streampos head = is.tellg();

	is.seekg(0, std::ios_base::end);
	std::streampos tail = is.tellg();

	is.seekg(0, std::ios_base::beg);
	int length = tail - head;

	file_data = new u1[length];
	if (file_data == NULL)
	{
		return 0;
	}
	// set position
	where_ = file_data;
	is.read((char*)file_data, length);
	is.close();
	return length;
}

void CClassFileParser::read_magic()
{
	memcpy(&pClassFile->magic, where_, sizeof(pClassFile->magic));
	pClassFile->magic = swapu4(pClassFile->magic);
	where_ += sizeof(pClassFile->magic);
}

void CClassFileParser::read_minorversion()
{
	memcpy(&pClassFile->minor_version, where_, sizeof(pClassFile->minor_version));
	pClassFile->minor_version = swapu2(pClassFile->minor_version);
	where_ += sizeof(pClassFile->minor_version);
}

void CClassFileParser::read_majorversion()
{
	memcpy(&pClassFile->major_version, where_, sizeof(pClassFile->major_version));
	pClassFile->major_version = swapu2(pClassFile->major_version);
	where_ += sizeof(pClassFile->major_version);
}

void CClassFileParser::read_constpoolsize()
{
	memcpy(&pClassFile->constant_pool_count, where_, sizeof(pClassFile->constant_pool_count));
	pClassFile->constant_pool_count = swapu2(pClassFile->constant_pool_count);
	where_ += sizeof(pClassFile->constant_pool_count);
}

void CClassFileParser::read_constpools()
{
	if (pClassFile->constant_pool_count == 0)
	{
		return;
	}

	pClassFile->constant_pool = new cp_info*[pClassFile->constant_pool_count];
	m_Ptrs.push_back(new Pointer('{', pClassFile->constant_pool_count, pClassFile->constant_pool));
	for (int i = 0; i < pClassFile->constant_pool_count-1; i++)
	{
		switch (*where_)
		{
		case CONSTANT_Class:
		{
			read_constant_class(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_Fieldref:
		{
			read_constant_fieldref(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_Methodref:
		{
			read_constant_methodref(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_InterfaceMethodref:
		{
			read_constant_interfacemethodref(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_String:
		{
			read_constant_string(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_Integer:
		{
			read_constant_integer(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_Float:
		{
			read_constant_float(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_Long:
		{
			read_constant_long(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_Double:
		{
			read_constant_double(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_NameAndType:
		{
			read_constant_nameandtype(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_Utf8:
		{
			read_constant_utf8(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_MethodHandle:
		{
			read_constant_methodhandle(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_MethodType:
		{
			read_constant_methodtype(&pClassFile->constant_pool[i]);
			break;
		}
		case CONSTANT_InvokeDynamic:
		{
			read_constant_invokedynamic(&pClassFile->constant_pool[i]);
			break;
		}
		}
	}
}

void CClassFileParser::read_accessflag()
{
	memcpy(&pClassFile->access_flags, where_, sizeof(pClassFile->access_flags));
	pClassFile->access_flags = swapu2(pClassFile->access_flags);
	where_ += sizeof(pClassFile->access_flags);
}

void CClassFileParser::read_thisclass()
{
	memcpy(&pClassFile->this_class, where_, sizeof(pClassFile->this_class));
	pClassFile->this_class = swapu2(pClassFile->this_class);
	where_ += sizeof(pClassFile->this_class);
}

void CClassFileParser::read_supperclass()
{
	memcpy(&pClassFile->super_class, where_, sizeof(pClassFile->super_class));
	pClassFile->super_class = swapu2(pClassFile->super_class);
	where_ += sizeof(pClassFile->super_class);
}

void CClassFileParser::read_interfacecount()
{
	memcpy(&pClassFile->interfaces_count, where_, sizeof(pClassFile->interfaces_count));
	pClassFile->interfaces_count = swapu2(pClassFile->interfaces_count);
	where_ += sizeof(pClassFile->interfaces_count);
}

void CClassFileParser::read_interfaces()
{
	if (pClassFile->interfaces_count > 0)
	{
		pClassFile->interfaces = new u2[pClassFile->interfaces_count];
		m_Ptrs.push_back(new Pointer('[', 0, pClassFile->interfaces));
		memcpy(pClassFile->interfaces, where_, sizeof(u2) * pClassFile->interfaces_count);
		where_ += sizeof(u2) * pClassFile->interfaces_count;
	}
}

void CClassFileParser::read_fieldscount()
{
	memcpy(&pClassFile->fields_count, where_, sizeof(pClassFile->fields_count));
	pClassFile->fields_count = swapu2(pClassFile->fields_count);
	where_ += sizeof(pClassFile->fields_count);
}

void CClassFileParser::read_fields()
{
	if (pClassFile->fields_count > 0)
	{
		pClassFile->fields = new field_info * [pClassFile->fields_count];
		m_Ptrs.push_back(new Pointer('{', pClassFile->fields_count, pClassFile->fields));
		for (int i = 0; i < pClassFile->fields_count; i++)
		{
			read_fieldsinfo(&pClassFile->fields[i]);
		}
	}
}

void CClassFileParser::read_methodscount()
{
	memcpy(&pClassFile->methods_count, where_, sizeof(pClassFile->methods_count));
	pClassFile->methods_count = swapu2(pClassFile->methods_count);
	where_ += sizeof(pClassFile->methods_count);
}

void CClassFileParser::read_methods()
{
	if (pClassFile->methods_count == 0)return;
	pClassFile->methods = new method_info * [pClassFile->methods_count];
	m_Ptrs.push_back(new Pointer('{', pClassFile->methods_count, pClassFile->methods));
	for (int i = 0; i < pClassFile->methods_count; i++)
	{
		read_methodsinfo(&pClassFile->methods[i]);
	}
}

void CClassFileParser::read_attributescount()
{
	memcpy(&pClassFile->attributes_count, where_, sizeof(pClassFile->attributes_count));
	pClassFile->attributes_count = swapu2(pClassFile->attributes_count);
	where_ += sizeof(pClassFile->attributes_count);
}

void CClassFileParser::read_attributes()
{
	if (pClassFile->attributes_count > 0)
	{
		pClassFile->attributes = new attribute_info * [pClassFile->attributes_count];
		m_Ptrs.push_back(new Pointer('{', pClassFile->attributes_count, pClassFile->attributes));
		for (int i = 0; i < pClassFile->attributes_count; i++)
		{
			read_attributes_attributeinfo(&pClassFile->attributes[i]);
		}
	}
}

void CClassFileParser::read_constant_class(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_Class_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(CONSTANT_Class_info));
	memcpy((*pinfo), where_, sizeof(CONSTANT_Class_info));
	((CONSTANT_Class_info*)(*pinfo))->name_index = swapu2(((CONSTANT_Class_info*)(*pinfo))->name_index);
	where_ += sizeof(CONSTANT_Class_info);
}

void CClassFileParser::read_constant_fieldref(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_Fieldref_info;
	m_Ptrs.push_back(new Pointer('o',0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(CONSTANT_Fieldref_info));
	memcpy((*pinfo), where_, sizeof(CONSTANT_Fieldref_info));
	where_ += sizeof(CONSTANT_Fieldref_info);
}

void CClassFileParser::read_constant_methodref(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_Methodref_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(CONSTANT_Methodref_info));
	memcpy((*pinfo), where_, sizeof(CONSTANT_Methodref_info));
	where_ += sizeof(CONSTANT_Methodref_info);
}

void CClassFileParser::read_constant_interfacemethodref(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_InterfaceMethodref_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(CONSTANT_InterfaceMethodref_info));
	memcpy((*pinfo), where_, sizeof(CONSTANT_InterfaceMethodref_info));
	where_ += sizeof(CONSTANT_InterfaceMethodref_info);
}

void CClassFileParser::read_constant_string(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_String_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(CONSTANT_String_info));
	memcpy((*pinfo), where_, sizeof(CONSTANT_String_info));
	((CONSTANT_String_info*)(*pinfo))->string_index = swapu2(((CONSTANT_String_info*)(*pinfo))->string_index);
	where_ += sizeof(CONSTANT_String_info);
}

void CClassFileParser::read_constant_integer(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_Integer_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(CONSTANT_Integer_info));
	memcpy((*pinfo), where_, sizeof(CONSTANT_Integer_info));
	where_ += sizeof(CONSTANT_Integer_info);
}

void CClassFileParser::read_constant_float(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_Float_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(CONSTANT_Float_info));
	memcpy((*pinfo), where_, sizeof(CONSTANT_Float_info));
	where_ += sizeof(CONSTANT_Float_info);
}

void CClassFileParser::read_constant_long(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_Long_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(CONSTANT_Long_info));
	memcpy((*pinfo), where_, sizeof(CONSTANT_Long_info));
	where_ += sizeof(CONSTANT_Long_info);
}

void CClassFileParser::read_constant_double(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_Double_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(CONSTANT_Double_info));
	memcpy((*pinfo), where_, sizeof(CONSTANT_Double_info));
	where_ += sizeof(CONSTANT_Double_info);
}

void CClassFileParser::read_constant_nameandtype(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_NameAndType_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(CONSTANT_NameAndType_info));
	memcpy((*pinfo), where_, sizeof(CONSTANT_NameAndType_info));
	where_ += sizeof(CONSTANT_NameAndType_info);
}

void CClassFileParser::read_constant_utf8(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_Utf8_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(CONSTANT_Utf8_info));
	
	// copy tag
	memcpy(&(*pinfo)->tag, where_, sizeof((*pinfo)->tag));
	where_ += sizeof((*pinfo)->tag);

	// copy length
	memcpy(&((CONSTANT_Utf8_info*)(*pinfo))->length, where_, sizeof(CONSTANT_Utf8_info::length));
	where_ += sizeof(CONSTANT_Utf8_info::length);
	if (((CONSTANT_Utf8_info*)(*pinfo))->length==0)
	{
		return;// no more bytes
	}

	// copy bytes
	((CONSTANT_Utf8_info*)(*pinfo))->length = swapu2(((CONSTANT_Utf8_info*)(*pinfo))->length);
	size_t totalsize = ((CONSTANT_Utf8_info*)(*pinfo))->length + sizeof(u1);
	((CONSTANT_Utf8_info*)(*pinfo))->bytes = new u1[totalsize];
	m_Ptrs.push_back(new Pointer('[', 0, ((CONSTANT_Utf8_info*)(*pinfo))->bytes));
	memset(((CONSTANT_Utf8_info*)(*pinfo))->bytes, 0x0, totalsize);
	memcpy(((CONSTANT_Utf8_info*)(*pinfo))->bytes, where_, ((CONSTANT_Utf8_info*)(*pinfo))->length);
	where_ += ((CONSTANT_Utf8_info*)(*pinfo))->length;
}

void CClassFileParser::read_constant_methodhandle(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_MethodHandle_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memcpy((*pinfo), where_, sizeof(CONSTANT_MethodHandle_info));
	where_ += sizeof(CONSTANT_MethodHandle_info);
}

void CClassFileParser::read_constant_methodtype(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_MethodType_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memcpy((*pinfo), where_, sizeof(CONSTANT_MethodType_info));
	where_ += sizeof(CONSTANT_MethodType_info);
}

void CClassFileParser::read_constant_invokedynamic(cp_info** pinfo)
{
	(*pinfo) = new CONSTANT_InvokeDynamic_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memcpy((*pinfo), where_, sizeof(CONSTANT_InvokeDynamic_info));
	where_ += sizeof(CONSTANT_InvokeDynamic_info);
}

inline u2 CClassFileParser::swapu2(u2 x)
{
	u2 data = ((((x) & 0x00ff) << 8) | (((x) & 0xff00) >> 8));
	return data;
}

inline u4 CClassFileParser::swapu4(u4 x)
{
	u4 data = (((x) & 0x000000ff) << 24) | (((x) & 0x0000FF00) << 8) | (((x) & 0x00FF0000) >> 8) | (((x) & 0xFF000000) >> 24);
	return data;
}

void CClassFileParser::read_fieldsinfo(field_info** pinfo)
{
	(*pinfo) = new field_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memcpy(&(*pinfo)->access_flags, where_, sizeof((*pinfo)->access_flags));
	where_ += sizeof((*pinfo)->access_flags);

	memcpy(&(*pinfo)->name_index, where_, sizeof((*pinfo)->name_index));
	(*pinfo)->name_index = swapu2((*pinfo)->name_index);
	where_ += sizeof((*pinfo)->name_index);

	memcpy(&(*pinfo)->descriptor_index, where_, sizeof((*pinfo)->descriptor_index));
	where_ += sizeof((*pinfo)->descriptor_index);

	memcpy(&(*pinfo)->attributes_count, where_, sizeof((*pinfo)->attributes_count));
	(*pinfo)->attributes_count = swapu2((*pinfo)->attributes_count);
	where_ += sizeof((*pinfo)->attributes_count);
	if ((*pinfo)->attributes_count == 0)
	{
		return;
	}

	(*pinfo)->attributes = new attribute_info * [(*pinfo)->attributes_count];
	m_Ptrs.push_back(new Pointer('{', (*pinfo)->attributes_count, (*pinfo)->attributes));
	for (int i = 0; i < (*pinfo)->attributes_count; i++)
	{
		read_field_attributeinfo(&(*pinfo)->attributes[i]);
	}
}

void CClassFileParser::read_field_attributeinfo(attribute_info** pinfo)
{
	// get attribute name index
	u2 attribute_name_index = 0;
	memcpy(&attribute_name_index, where_, sizeof(attribute_name_index));
	attribute_name_index = swapu2(attribute_name_index);
	char* attr_name = get_attribute_name(attribute_name_index-1);
	if (strcmp("ConstantValue", attr_name) == 0)
	{
		read_ConstantValue_attrinfo(pinfo);
	}
	else if (strcmp("Synthetic", attr_name) == 0)
	{
		read_Synthetic_attrinfo(pinfo);
	}
	else if (strcmp("Signature", attr_name) == 0)
	{
		read_Signature_attrinfo(pinfo);
	}
	else if (strcmp("Deprecated", attr_name) == 0)
	{
		read_Deprecated_attrinfo(pinfo);
	}
	else if (strcmp("RuntimeVisibleAnnotations", attr_name) == 0)
	{
		read_RuntimeVisibleAnnotations_attrinfo(pinfo);
	}
	else if (strcmp("RuntimeInvisibleAnnotations", attr_name) == 0)
	{
		read_RuntimeInvisibleAnnotations_attrinfo(pinfo);
	}
}

char* CClassFileParser::get_attribute_name(u2 name_index)
{
	CONSTANT_Utf8_info* info = (CONSTANT_Utf8_info*)pClassFile->constant_pool[name_index];
	return (char*)info->bytes;
}

void CClassFileParser::read_ConstantValue_attrinfo(attribute_info** pinfo)
{
	(*pinfo) = new ConstantValue_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memcpy((*pinfo), where_, sizeof(ConstantValue_attribute));
	where_ += sizeof(ConstantValue_attribute);
}

void CClassFileParser::read_Synthetic_attrinfo(attribute_info** pinfo)
{
	(*pinfo) = new Synthetic_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memcpy((*pinfo), where_, sizeof(Synthetic_attribute));
	where_ += sizeof(Synthetic_attribute);
}

void CClassFileParser::read_Signature_attrinfo(attribute_info** pinfo)
{
	(*pinfo) = new Signature_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memcpy((*pinfo), where_, sizeof(Signature_attribute));
	where_ += sizeof(Signature_attribute);
}

void CClassFileParser::read_Deprecated_attrinfo(attribute_info** pinfo)
{
	(*pinfo) = new Deprecated_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memcpy((*pinfo), where_, sizeof(Deprecated_attribute));
	where_ += sizeof(Deprecated_attribute);
}

void CClassFileParser::read_RuntimeVisibleAnnotations_attrinfo(attribute_info** pinfo)
{
	(*pinfo) = new RuntimeVisibleAnnotations_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	// attribute name index
	memcpy(&((RuntimeVisibleAnnotations_attribute*)(*pinfo))->attribute_name_index,
		where_,
		sizeof(((RuntimeVisibleAnnotations_attribute*)(*pinfo))->attribute_name_index)
	);
	where_ += sizeof(((RuntimeVisibleAnnotations_attribute*)(*pinfo))->attribute_name_index);

	// attribute length
	memcpy(&((RuntimeVisibleAnnotations_attribute*)(*pinfo))->attribute_length,
		where_,
		sizeof(((RuntimeVisibleAnnotations_attribute*)(*pinfo))->attribute_length)
	);
	where_ += sizeof(((RuntimeVisibleAnnotations_attribute*)(*pinfo))->attribute_length);

	// number of annotations
	memcpy(&((RuntimeVisibleAnnotations_attribute*)(*pinfo))->num_annotations,
		where_,
		sizeof(((RuntimeVisibleAnnotations_attribute*)(*pinfo))->num_annotations)
	);

	((RuntimeVisibleAnnotations_attribute*)(*pinfo))->num_annotations = swapu2(
		((RuntimeVisibleAnnotations_attribute*)(*pinfo))->num_annotations
	);
	where_ += sizeof(((RuntimeVisibleAnnotations_attribute*)(*pinfo))->num_annotations);

	if (((RuntimeVisibleAnnotations_attribute*)(*pinfo))->num_annotations > 0)
	{
		((RuntimeVisibleAnnotations_attribute*)(*pinfo))->annotations = 
			new annotation * [((RuntimeVisibleAnnotations_attribute*)(*pinfo))->num_annotations];
		m_Ptrs.push_back(new Pointer('{',
			((RuntimeVisibleAnnotations_attribute*)(*pinfo))->num_annotations, 
			((RuntimeVisibleAnnotations_attribute*)(*pinfo))->annotations));
		// read all annotation
		for (int i = 0; i < ((RuntimeVisibleAnnotations_attribute*)(*pinfo))->num_annotations; i++)
		{
			read_annotation(&((RuntimeVisibleAnnotations_attribute*)(*pinfo))->annotations[i]);
		}
	}
}

void CClassFileParser::read_RuntimeInvisibleAnnotations_attrinfo(attribute_info** pinfo)
{
	read_RuntimeVisibleAnnotations_attrinfo(pinfo);
}

void CClassFileParser::read_annotation(annotation** pinfo)
{
	(*pinfo) = new annotation;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(annotation));
	
	// type index
	memcpy(&(*pinfo)->type_index, where_, sizeof((*pinfo)->type_index));
	where_ += sizeof((*pinfo)->type_index);

	// number of element value pairs
	memcpy(&(*pinfo)->num_element_value_pairs, where_, sizeof((*pinfo)->num_element_value_pairs));
	(*pinfo)->num_element_value_pairs = swapu2((*pinfo)->num_element_value_pairs);
	where_ += sizeof((*pinfo)->num_element_value_pairs);
	if ((*pinfo)->num_element_value_pairs == 0)
	{
		return;
	}

	// read all element_value_pairs
	(*pinfo)->pairs = new element_value_pairs * [(*pinfo)->num_element_value_pairs];
	m_Ptrs.push_back(new Pointer('{', (*pinfo)->num_element_value_pairs, (*pinfo)->pairs));
	for (int i = 0; i < (*pinfo)->num_element_value_pairs; i++)
	{
		read_element_value_pair(&(*pinfo)->pairs[i]);
	}
}

void CClassFileParser::read_element_value_pair(element_value_pairs** pair)
{
	(*pair) = new element_value_pairs;
	m_Ptrs.push_back(new Pointer('o', 0, (*pair)));
	memset((*pair), 0x0, sizeof(element_value_pairs));
	
	// element name index
	memcpy(&(*pair)->element_name_index, where_, sizeof((*pair)->element_name_index));
	where_ += sizeof((*pair)->element_name_index);
	read_elementvalue(&(*pair)->value);
}

void CClassFileParser::read_arrayvalue(array_value** value)
{
	(*value) = new array_value;
	m_Ptrs.push_back(new Pointer('o', 0, (*value)));
	// number of values
	memcpy(&(*value)->num_values, where_, sizeof((*value)->num_values));
	(*value)->num_values = swapu2((*value)->num_values);
	where_ += sizeof((*value)->num_values);
	if ((*value)->num_values == 0) return;

	// values
	(*value)->values = new element_value * [(*value)->num_values];
	m_Ptrs.push_back(new Pointer('{', (*value)->num_values, (*value)->values));
	for (int i = 0; i < (*value)->num_values; i++)
	{
		read_elementvalue(&(*value)->values[i]);
	}
}

void CClassFileParser::read_elementvalue(element_value** value)
{
	// element value tag
	(*value) = new element_value;
	m_Ptrs.push_back(new Pointer('o', 0, (*value)));
	memset(value, 0x0, sizeof(element_value));
	memcpy(&(*value)->tag, where_, sizeof((*value)->tag));
	where_ += sizeof((*value)->tag);
	switch ((*value)->tag)
	{
	case 's':
	case 'B':
	case 'C':
	case 'D':
	case 'F':
	case 'I':
	case 'J':
	case 'S':
	case 'Z':
	{
		memcpy(&(*value)->value.const_value_index, where_, sizeof(u2));
		where_ += sizeof((*value)->value.const_value_index);
		break;
	}

	case 'e':
	{
		memcpy(&(*value)->value.e_const_value, where_, sizeof(enum_const_value));
		where_ += sizeof(enum_const_value);
		break;
	}

	case 'c':
	{
		memcpy(&(*value)->value.class_info_index, where_, sizeof(u2));
		where_ += sizeof((*value)->value.class_info_index);
		break;
	}

	case '@':
	{
		read_annotation(&(*value)->value.annotation_value);
		break;
	}

	case '[':
	{
		read_arrayvalue(&((*value)->value.arrayvalue));
		break;
	}

	default:
		break;//unknown
	}

}

void CClassFileParser::read_methodsinfo(method_info** pinfo)
{
	(*pinfo) = new method_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	// access flag
	memcpy(&(*pinfo)->access_flags, where_, sizeof((*pinfo)->access_flags));
	where_ += sizeof((*pinfo)->access_flags);

	// name index
	memcpy(&(*pinfo)->name_index, where_, sizeof((*pinfo)->name_index));
	(*pinfo)->name_index = swapu2((*pinfo)->name_index);
	where_ += sizeof((*pinfo)->name_index);

	// descriptor index
	memcpy(&(*pinfo)->descriptor_index, where_, sizeof((*pinfo)->descriptor_index));
	(*pinfo)->descriptor_index = swapu2((*pinfo)->descriptor_index);
	where_ += sizeof((*pinfo)->descriptor_index);

	// number of attribute
	memcpy(&(*pinfo)->attributes_count, where_, sizeof((*pinfo)->attributes_count));
	(*pinfo)->attributes_count = swapu2((*pinfo)->attributes_count);
	where_ += sizeof((*pinfo)->attributes_count);
	if ((*pinfo)->attributes_count == 0)return;

	// read method attributes
	(*pinfo)->attributes = new attribute_info * [(*pinfo)->attributes_count];
	m_Ptrs.push_back(new Pointer('{', (*pinfo)->attributes_count, (*pinfo)->attributes));
	for (int i = 0; i < (*pinfo)->attributes_count; i++)
	{
		read_method_attributeinfo(&(*pinfo)->attributes[i]);
	}
}

void CClassFileParser::read_method_attributeinfo(attribute_info** pinfo)
{
	(*pinfo) = new attribute_info;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(attribute_info));
	// attrubite name index
	u2 attribute_name_index = 0;
	memcpy(&attribute_name_index, where_, sizeof(attribute_name_index));
	attribute_name_index = swapu2(attribute_name_index);
	char* attr_name = get_attribute_name(attribute_name_index-1);
	if (attr_name == 0)return;

	if (strcmp("Code", attr_name) == 0)
	{
		read_code_attribute(pinfo); return;
	}
	else if (strcmp("Exceptions", attr_name) == 0)
	{
		read_exception_attribute(pinfo); return;
	}
	else if (strcmp("Synthetic", attr_name) == 0)
	{
		read_Synthetic_attrinfo(pinfo); return;
	}
	else if (strcmp("Signature", attr_name) == 0)
	{
		read_Signature_attrinfo(pinfo); return;
	}
	else if (strcmp("Deprecated", attr_name) == 0)
	{
		read_Deprecated_attrinfo(pinfo); return;
	}
	else if (strcmp("RuntimeVisibleAnnotations", attr_name) == 0)
	{
		read_RuntimeVisibleAnnotations_attrinfo(pinfo); return;
	}
	else if (strcmp("RuntimeInvisibleAnnotations", attr_name) == 0)
	{
		read_RuntimeInvisibleAnnotations_attrinfo(pinfo); return;
	}
	else if (strcmp("RuntimeVisibleParameterAnnotations", attr_name) == 0)
	{
		read_RuntimeVisibleParameterAnnotations_attrinfo(pinfo); return;
	}
	else if (strcmp("RuntimeInvisibleParameterAnnotations", attr_name) == 0)
	{
		read_RuntimeInvisibleParameterAnnotations_attrinfo(pinfo); return;
	}
	else if (strcmp("AnnotationDefault", attr_name) == 0)
	{
		read_AnnotationDefault_attrinfo(pinfo);
	}
}

void CClassFileParser::read_code_attribute(attribute_info** pinfo)
{
	(*pinfo) = new Code_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(Code_attribute));
	Code_attribute* code = (Code_attribute*)(*pinfo);

	// attribute name index
	memcpy(&code->attribute_name_index, where_, sizeof(code->attribute_name_index));
	where_ += sizeof(code->attribute_name_index);

	// attribute length
	memcpy(&code->attribute_length, where_, sizeof(code->attribute_length));
	where_ += sizeof(code->attribute_length);

	// max stack
	memcpy(&code->max_stack, where_, sizeof(code->max_stack));
	where_ += sizeof(code->max_stack);

	// max locals
	memcpy(&code->max_locals, where_, sizeof(code->max_locals));
	where_ += sizeof(code->max_locals);

	// code length
	memcpy(&code->code_length, where_, sizeof(code->code_length));
	code->code_length = swapu4(code->code_length);
	where_ += sizeof(code->code_length);

	// code
	code->code = new u1[code->code_length];
	m_Ptrs.push_back(new Pointer('[', code->code_length, code->code));
	memcpy(code->code, where_, code->code_length);
	where_ += code->code_length;

	// exception table length
	memcpy(&code->exception_table_length, where_, sizeof(code->exception_table_length));
	where_ += sizeof(code->exception_table_length);
	code->exception_table_length = swapu2(code->exception_table_length);
	if (code->exception_table_length > 0)
	{
		code->exc_tables = new exception_table * [code->exception_table_length];
		m_Ptrs.push_back(new Pointer('{', code->exception_table_length, code->exc_tables));
		for (int i = 0; i < code->exception_table_length; i++)
		{
			read_exception_table(&code->exc_tables[i]);
		}
	}

	// attributes count
	memcpy(&code->attributes_count, where_, sizeof(code->attributes_count));
	code->attributes_count = swapu2(code->attributes_count);
	where_ += sizeof(code->attributes_count);
	if (code->attributes_count > 0)
	{
		code->attributes = new attribute_info * [code->attributes_count];
		m_Ptrs.push_back(new Pointer('{', code->attributes_count, code->attributes));
		for (int j = 0; j < code->attributes_count; j++)
		{
			read_code_attributeinfo(&code->attributes[j]);
		}
	}
}

void CClassFileParser::read_exception_attribute(attribute_info** pinfo)
{
	(*pinfo) = new Exceptions_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(Exceptions_attribute));

	Exceptions_attribute* p = (Exceptions_attribute*)(*pinfo);

	// atrubite name index
	memcpy(&p->attribute_name_index, where_, sizeof(p->attribute_name_index));
	p->attribute_name_index = swapu2(p->attribute_name_index);
	where_ += sizeof(p->attribute_name_index);

	// attribute length
	memcpy(&p->attribute_length, where_, sizeof(p->attribute_length));
	p->attribute_length = swapu4(p->attribute_length);
	where_ += sizeof(p->attribute_length);

	// number of exceptions
	memcpy(&p->number_of_exceptions, where_, sizeof(p->number_of_exceptions));
	p->number_of_exceptions = swapu2(p->number_of_exceptions);
	where_ += sizeof(p->number_of_exceptions);
	if (p->number_of_exceptions == 0)return;

	// read all exception index
	p->exception_index_table = new u2[p->number_of_exceptions];
	m_Ptrs.push_back(new Pointer('[', p->number_of_exceptions, p->exception_index_table));
	memcpy(p->exception_index_table, where_, sizeof(u2) * p->number_of_exceptions);	
}

void CClassFileParser::read_RuntimeVisibleParameterAnnotations_attrinfo(attribute_info** pinfo)
{
	(*pinfo) = new RuntimeVisibleParameterAnnotations_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(RuntimeVisibleParameterAnnotations_attribute));
	RuntimeVisibleParameterAnnotations_attribute* p = (RuntimeVisibleParameterAnnotations_attribute*)(*pinfo);

	// attribute name index
	memcpy(&p->attribute_name_index, where_, sizeof(p->attribute_name_index));
	p->attribute_name_index = swapu2(p->attribute_name_index);
	where_ += sizeof(p->attribute_name_index);

	// attribute length
	memcpy(&p->attribute_length, where_, sizeof(p->attribute_length));
	where_ += sizeof(p->attribute_length);

	// number of parameters
	memcpy(&p->num_parameters, where_, sizeof(p->num_parameters));
	where_ += sizeof(p->num_parameters);
	if (p->num_parameters == 0)return;

	// pararmeter anntations
	p->parameter_annotations = new parameter_annotation * [p->num_parameters];
	m_Ptrs.push_back(new Pointer('{', p->num_parameters, p->parameter_annotations));
	for (int i = 0; i < p->num_parameters; i++)
	{
		read_parameter_annotation(&p->parameter_annotations[i]);
	}
}

void CClassFileParser::read_RuntimeInvisibleParameterAnnotations_attrinfo(attribute_info** pinfo)
{
	read_RuntimeVisibleParameterAnnotations_attrinfo(pinfo);
}

void CClassFileParser::read_AnnotationDefault_attrinfo(attribute_info** pinfo)
{
	(*pinfo) = new AnnotationDefault_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(AnnotationDefault_attribute));

	AnnotationDefault_attribute* p = (AnnotationDefault_attribute*)(*pinfo);

	memcpy(&p->attribute_name_index, where_, sizeof(p->attribute_name_index));
	where_ += sizeof(p->attribute_name_index);

	memcpy(&p->attribute_length, where_, sizeof(p->attribute_length));
	where_ += sizeof(p->attribute_length);
	read_elementvalue(&p->default_value);
}

void CClassFileParser::read_exception_table(exception_table** ptable)
{
	(*ptable) = new exception_table;
	m_Ptrs.push_back(new Pointer('o', 0, (*ptable)));
	memcpy((*ptable), where_, sizeof(exception_table));
	where_ += sizeof(exception_table);
}

void CClassFileParser::read_code_attributeinfo(attribute_info** pinfo)
{
	u2 attribute_name_index = 0;
	memcpy(&attribute_name_index, where_, sizeof(attribute_name_index));
	attribute_name_index = swapu2(attribute_name_index);
	char* attr_name = get_attribute_name(attribute_name_index-1);
	if (attr_name == 0) return;
	if (strcmp("LineNumberTable", attr_name) == 0)
	{
		read_LineNumberTable_attribute(pinfo);return;
	}
	else if (strcmp("LocalVariableTable", attr_name) == 0)
	{
		read_LocalVariableTable_attribute(pinfo); return;
	}
	else if (strcmp("LocalVariableTypeTable", attr_name) == 0)
	{
		read_LocalVariableTypeTable_attribute(pinfo); return;
	}
	else if (strcmp("StackMapTable", attr_name) == 0)
	{
		read_StackMapTable_attribute(pinfo);
	}
}

void CClassFileParser::read_LineNumberTable_attribute(attribute_info** pinfo)
{
	(*pinfo) = new LineNumberTable_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(LineNumberTable_attribute));
	
	memcpy(&(*pinfo)->attribute_name_index, where_, sizeof((*pinfo)->attribute_name_index));
	(*pinfo)->attribute_name_index = swapu2((*pinfo)->attribute_name_index);
	where_ += sizeof((*pinfo)->attribute_name_index);

	memcpy(&(*pinfo)->attribute_length, where_, sizeof((*pinfo)->attribute_length));
	(*pinfo)->attribute_length = swapu4((*pinfo)->attribute_length);
	where_ += sizeof((*pinfo)->attribute_length);

	memcpy(&((LineNumberTable_attribute*)(*pinfo))->line_number_table_length, where_, sizeof(u2));
	((LineNumberTable_attribute*)(*pinfo))->line_number_table_length = swapu2(
		((LineNumberTable_attribute*)(*pinfo))->line_number_table_length
	);
	where_ += sizeof(u2);

	if (((LineNumberTable_attribute*)(*pinfo))->line_number_table_length == 0)
	{
		return;
	}

	int N = ((LineNumberTable_attribute*)(*pinfo))->line_number_table_length;
	((LineNumberTable_attribute*)(*pinfo))->line_number_tables = new line_number_table * [N];
	m_Ptrs.push_back(new Pointer('{', N, ((LineNumberTable_attribute*)(*pinfo))->line_number_tables));
	for (int i = 0; i < N; i++)
	{
		read_line_number_table(&((LineNumberTable_attribute*)(*pinfo))->line_number_tables[i]);
	}
}

void CClassFileParser::read_LocalVariableTable_attribute(attribute_info** pinfo)
{
	(*pinfo) = new LocalVariableTable_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset(*pinfo, 0x0, sizeof(LocalVariableTable_attribute));

	// attribute name index
	memcpy(&(*pinfo)->attribute_name_index, where_, sizeof((*pinfo)->attribute_name_index));
	where_ += sizeof((*pinfo)->attribute_name_index);

	// attribute length
	memcpy(&(*pinfo)->attribute_length, where_, sizeof((*pinfo)->attribute_length));
	where_ += sizeof((*pinfo)->attribute_length);

	// local variable table length
	LocalVariableTable_attribute* p = (LocalVariableTable_attribute*)(*pinfo);
	memcpy(&p->local_variable_table_length, where_, sizeof(p->local_variable_table_length));
	p->local_variable_table_length = swapu2(p->local_variable_table_length);
	where_ += sizeof(p->local_variable_table_length);
	if (p->local_variable_table_length == 0)return;

	// local variable tables
	p->tables = new local_variable_table * [p->local_variable_table_length];
	m_Ptrs.push_back(new Pointer('{', p->local_variable_table_length, p->tables));
	for (int i = 0; i < p->local_variable_table_length; i++)
	{
		read_local_variable_table(&p->tables[i]);
	}
}

void CClassFileParser::read_LocalVariableTypeTable_attribute(attribute_info** pinfo)
{
	(*pinfo) = new LocalVariableTypeTable_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(LocalVariableTypeTable_attribute));
	LocalVariableTypeTable_attribute* p = (LocalVariableTypeTable_attribute*)(*pinfo);

	// attribute name index
	memcpy(&p->attribute_name_index, where_, sizeof(p->attribute_name_index));
	where_ += sizeof(p->attribute_name_index);

	// attribute length
	memcpy(&p->attribute_length, where_, sizeof(p->attribute_length));
	where_ += sizeof((*pinfo)->attribute_length);

	// local variable table length
	memcpy(&p->local_variable_type_table_length, where_, sizeof(p->local_variable_type_table_length));
	p->local_variable_type_table_length = swapu2(p->local_variable_type_table_length);
	where_ += sizeof(p->local_variable_type_table_length);
	if (p->local_variable_type_table_length == 0)return;

	// local variable tables
	p->tables = new local_variable_type_table * [p->local_variable_type_table_length];
	m_Ptrs.push_back(new Pointer('{', p->local_variable_type_table_length, p->tables));
	for (int i = 0; i < p->local_variable_type_table_length; i++)
	{
		read_local_variable_type_table(&p->tables[i]);
	}
}

void CClassFileParser::read_StackMapTable_attribute(attribute_info** pinfo)
{
	(*pinfo) = new StackMapTable_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	StackMapTable_attribute* p = (StackMapTable_attribute*)(*pinfo);
	memset(p, 0x0, sizeof(StackMapTable_attribute));

	//attribute name index
	memcpy(&p->attribute_name_index, where_, sizeof(p->attribute_name_index));
	p->attribute_name_index = swapu2(p->attribute_name_index);
	where_ += sizeof(p->attribute_name_index);

	// attribute length
	memcpy(&p->attribute_length, where_, sizeof(p->attribute_length));
	p->attribute_length = swapu4(p->attribute_length);
	where_ += sizeof(p->attribute_length);

	// number of entries
	memcpy(&p->number_of_entries, where_, sizeof(p->number_of_entries));
	p->number_of_entries = swapu2(p->number_of_entries);
	where_ += sizeof(p->number_of_entries);
	if (p->number_of_entries == 0)return;

	// read all entries
	p->entries = new stack_map_frame * [p->number_of_entries];
	m_Ptrs.push_back(new Pointer('o', p->number_of_entries, p->entries));
	for (int i = 0; i < p->number_of_entries; i++)
	{
		read_stack_map_frame(&p->entries[i]);
	}
}

void CClassFileParser::read_line_number_table(line_number_table** ptable)
{
	(*ptable) = new line_number_table;
	m_Ptrs.push_back(new Pointer('o', 0, (*ptable)));
	memcpy((*ptable), where_, sizeof(line_number_table));
	where_ += sizeof(line_number_table);
}

void CClassFileParser::read_local_variable_table(local_variable_table** ptable)
{
	(*ptable) = new local_variable_table;
	m_Ptrs.push_back(new Pointer('o', 0, (*ptable)));
	memset((*ptable), 0x0, sizeof(local_variable_table));
	memcpy((*ptable), where_, sizeof(local_variable_table));
	where_ += sizeof(local_variable_table);
}

void CClassFileParser::read_local_variable_type_table(local_variable_type_table** ptable)
{
	(*ptable) = new local_variable_type_table;
	m_Ptrs.push_back(new Pointer('o', 0, (*ptable)));
	memset((*ptable), 0x0, sizeof(local_variable_type_table));
	memcpy((*ptable), where_, sizeof(local_variable_type_table));
	where_ += sizeof(local_variable_type_table);
}

void CClassFileParser::read_stack_map_frame(stack_map_frame** frame)
{
	(*frame) = new stack_map_frame;
	m_Ptrs.push_back(new Pointer('o', 0, (*frame)));
	// stack map frame 
	u1 tag = *(where_);
	if (tag >= 0 && tag <= 63)
	{
		memcpy(&(*frame)->sameframe, where_, sizeof(same_frame));
		where_ += sizeof(same_frame);
		return;
	}
	else if (tag >= 64 && tag <= 127)
	{
		memcpy(&(*frame)->sl1sif.frame_type, where_, sizeof((*frame)->sl1sif.frame_type));
		where_ += sizeof((*frame)->sl1sif.frame_type);
		memcpy(&(*frame)->sl1sif.stack[0], where_, sizeof(verification_type_info));
		where_ += sizeof(verification_type_info);
		return;
	}
	else if (tag == 247)
	{
		memcpy(&(*frame)->sl1sife.frame_type, where_, sizeof((*frame)->sl1sife.frame_type));
		where_ += sizeof((*frame)->sl1sife.frame_type);

		memcpy(&(*frame)->sl1sife.offset_delta, where_, sizeof(u2));
		where_ += sizeof(u2);

		memcpy(&(*frame)->sl1sife.stack[0], where_, sizeof(verification_type_info));
		where_ += sizeof(verification_type_info);
		return;
	}
	else if (tag >= 248 && tag <= 250)
	{
		memcpy(&(*frame)->chopframe.frame_type, where_, sizeof(u1));
		where_ += sizeof(u1);

		memcpy(&(*frame)->chopframe.offset_delta, where_, sizeof(u2));
		where_ += sizeof(u2);

		return;
	}
	else if (tag == 251)
	{
		memcpy(&(*frame)->sfe.frame_type, where_, sizeof(u1));
		where_ += sizeof(u1);

		memcpy(&(*frame)->sfe.offset_delta, where_, sizeof(u2));
		where_ += sizeof(u2);

		return;
	}
	else if (tag >= 252 && tag <= 254)
	{
		memcpy(&(*frame)->appendframe.frame_type, where_, sizeof(u1));
		where_ += sizeof(u1);

		memcpy(&(*frame)->appendframe.offset_delta, where_, sizeof(u2));
		where_ += sizeof(u2);

		int num_locals = (*frame)->appendframe.frame_type - 251;
		num_locals = swapu2(num_locals);
		if (num_locals > 0)
		{
			(*frame)->appendframe.locals = new verification_type_info * [num_locals];
			m_Ptrs.push_back(new Pointer('{', num_locals, (*frame)->appendframe.locals));
			for (int i = 0; i < num_locals; i++)
			{
				read_verification_type_info(&(*frame)->appendframe.locals[i]);
			}			
		}
		return;
	}
	else if (tag == 255)
	{
		memcpy(&(*frame)->fullframe.frame_type, where_, sizeof(u1));
		where_ += sizeof(u1);

		memcpy(&(*frame)->fullframe.offset_delta, where_, sizeof(u2));
		where_ += sizeof(u2);
		
		memcpy(&(*frame)->fullframe.number_of_locals, where_, sizeof(u2));
		where_ += sizeof(u2);

		int num = swapu2((*frame)->fullframe.number_of_locals);
		if (num > 0)
		{
			(*frame)->fullframe.locals = new verification_type_info * [num];
			m_Ptrs.push_back(new Pointer('{', num, (*frame)->fullframe.locals));
			for (int i = 0; i < num; i++)
			{
				read_verification_type_info(&(*frame)->fullframe.locals[i]);
			}
		}

		memcpy(&(*frame)->fullframe.number_of_stack_items, where_, sizeof(u2));
		where_ += sizeof(u2);
		num = swapu2((*frame)->fullframe.number_of_stack_items);
		if (num > 0)
		{
			(*frame)->fullframe.stack = new verification_type_info * [num];
			m_Ptrs.push_back(new Pointer('{', num, (*frame)->fullframe.stack));
			for (int j = 0; j < num; j++)
			{
				read_verification_type_info(&(*frame)->fullframe.stack[j]);
			}
		}
	}
}

void CClassFileParser::read_parameter_annotation(parameter_annotation** ppa)
{
	(*ppa) = new parameter_annotation;
	m_Ptrs.push_back(new Pointer('o', 0, (*ppa)));
	memset((*ppa), 0x0, sizeof(parameter_annotation));

	// number of annotations
	memcpy(&(*ppa)->num_annotations, where_, sizeof((*ppa)->num_annotations));
	(*ppa)->num_annotations = swapu2((*ppa)->num_annotations);
	where_ += sizeof((*ppa)->num_annotations);
	if ((*ppa)->num_annotations == 0)return;

	// read annotations
	(*ppa)->annotations = new annotation * [(*ppa)->num_annotations];
	m_Ptrs.push_back(new Pointer('{', (*ppa)->num_annotations, (*ppa)->annotations));
	for(int i=0; i< (*ppa)->num_annotations; i++)
	{
		read_annotation(&(*ppa)->annotations[i]);
	}
}

void CClassFileParser::read_attributes_attributeinfo(attribute_info** pinfo)
{
	u2 attribute_name_index = 0;
	memcpy(&attribute_name_index, where_, sizeof(attribute_name_index));
	attribute_name_index = swapu2(attribute_name_index);
	char* attr_name = get_attribute_name(attribute_name_index-1);
	if (attr_name == 0) return;
	if (strcmp("InnerClasses", attr_name) == 0)
	{
		read_InnerClasses_attribute(pinfo); return;
	}
	else if (strcmp("EnclosingMethod", attr_name) == 0)
	{
		read_EnclosingMethod_attribute(pinfo); return;
	}
	else if (strcmp("Synthetic", attr_name) == 0)
	{
		read_Synthetic_attrinfo(pinfo); return;
	}
	else if (strcmp("Signature", attr_name) == 0)
	{
		read_Signature_attrinfo(pinfo);
	}
	else if (strcmp("SourceFile", attr_name) == 0)
	{
		read_SourceFile_attrinfo(pinfo);
	}
	else if (strcmp("SourceDebugExtension", attr_name) == 0)
	{
		read_SourceDebugExtension_attrinfo(pinfo);
	}
	else if (strcmp("Deprecated", attr_name) == 0)
	{
		read_Deprecated_attrinfo(pinfo);
	}
	else if (strcmp("RuntimeVisibleAnnotations", attr_name) == 0)
	{
		read_RuntimeVisibleAnnotations_attrinfo(pinfo);
	}
	else if (strcmp("RuntimeInvisibleAnnotations", attr_name) == 0)
	{
		read_RuntimeInvisibleAnnotations_attrinfo(pinfo);
	}
	else if (strcmp("BootstrapMethods", attr_name) == 0)
	{
		read_BootstrapMethods_attrinfo(pinfo);
	}
}

void CClassFileParser::read_InnerClasses_attribute(attribute_info** pinfo)
{
	(*pinfo) = new InnerClasses_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	InnerClasses_attribute* p = (InnerClasses_attribute*)(*pinfo);

	memcpy(&p->attribute_name_index, where_, sizeof(p->attribute_name_index));
	where_ += sizeof(p->attribute_name_index);

	memcpy(&p->attribute_length, where_, sizeof(p->attribute_length));
	where_ += sizeof(p->attribute_length);

	memcpy(&p->number_of_classes, where_, sizeof(p->number_of_classes));
	p->number_of_classes = swapu2(p->number_of_classes);
	where_ += sizeof(p->number_of_classes);
	if (p->number_of_classes == 0)return;

	p->classes = new inner_class * [p->number_of_classes];
	m_Ptrs.push_back(new Pointer('{', p->number_of_classes, p->classes));
	for (int i = 0; i < p->number_of_classes; i++)
	{
		read_innerclass(&p->classes[i]);
	}
}

void CClassFileParser::read_EnclosingMethod_attribute(attribute_info** pinfo)
{
	(*pinfo) = new EnclosingMethod_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(EnclosingMethod_attribute));
	memcpy((*pinfo), where_, sizeof(EnclosingMethod_attribute));
	where_ += sizeof(EnclosingMethod_attribute);
}

void CClassFileParser::read_SourceFile_attrinfo(attribute_info** pinfo)
{
	(*pinfo) = new SourceFile_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(SourceFile_attribute));
	memcpy((*pinfo), where_, sizeof(SourceFile_attribute));
	where_ += sizeof(SourceFile_attribute);
}

void CClassFileParser::read_SourceDebugExtension_attrinfo(attribute_info** pinfo)
{
	(*pinfo) = new SourceDebugExtension_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset((*pinfo), 0x0, sizeof(SourceDebugExtension_attribute));
	SourceDebugExtension_attribute* p = (SourceDebugExtension_attribute*)(*pinfo);

	memcpy(&p->attribute_name_index, where_, sizeof(p->attribute_name_index));
	where_ += sizeof(p->attribute_name_index);

	memcpy(&p->attribute_length, where_, sizeof(p->attribute_length));
	p->attribute_length = swapu4(p->attribute_length);
	where_ += sizeof(p->attribute_length);

	if (p->attribute_length == 0)return;
	
	p->debug_extension = new u1[p->attribute_length];
	m_Ptrs.push_back(new Pointer('[', p->attribute_length, p->debug_extension));
	memcpy(p->debug_extension, where_, p->attribute_length);
	where_ += p->attribute_length;
}

void CClassFileParser::read_BootstrapMethods_attrinfo(attribute_info** pinfo)
{
	(*pinfo) = new BootstrapMethods_attribute;
	m_Ptrs.push_back(new Pointer('o', 0, (*pinfo)));
	memset(pinfo, 0x0, sizeof(BootstrapMethods_attribute));

	memcpy(&(*pinfo)->attribute_name_index, where_, sizeof((*pinfo)->attribute_name_index));
	where_ += sizeof((*pinfo)->attribute_name_index);

	memcpy(&(*pinfo)->attribute_length, where_, sizeof((*pinfo)->attribute_length));
	where_ += sizeof((*pinfo)->attribute_length);

	memcpy(&((BootstrapMethods_attribute*)*pinfo)->num_bootstrap_methods, 
		where_, 
		sizeof(((BootstrapMethods_attribute*)*pinfo)->num_bootstrap_methods)
	);
	((BootstrapMethods_attribute*)*pinfo)->num_bootstrap_methods = swapu2(((BootstrapMethods_attribute*)*pinfo)->num_bootstrap_methods);
	where_ += sizeof(((BootstrapMethods_attribute*)*pinfo)->num_bootstrap_methods);
	if (((BootstrapMethods_attribute*)*pinfo)->num_bootstrap_methods == 0) return;

	((BootstrapMethods_attribute*)*pinfo)->bootstrap_methods = 
		new bootstrap_method * [((BootstrapMethods_attribute*)*pinfo)->num_bootstrap_methods];
	m_Ptrs.push_back(new Pointer('{', 
		((BootstrapMethods_attribute*)*pinfo)->num_bootstrap_methods, 
		((BootstrapMethods_attribute*)*pinfo)->bootstrap_methods)
	);
	for (int i = 0; i < ((BootstrapMethods_attribute*)*pinfo)->num_bootstrap_methods; i++)
	{
		read_bootstrap_method(&((BootstrapMethods_attribute*)*pinfo)->bootstrap_methods[i]);
	}
}

void CClassFileParser::read_innerclass(inner_class** ic)
{
	*ic = new inner_class;
	m_Ptrs.push_back(new Pointer('o', 0, *ic));
	memset(*ic, 0x0, sizeof(inner_class));
	memcpy(*ic, where_, sizeof(inner_class));
	where_ += sizeof(inner_class);
}

void CClassFileParser::read_bootstrap_method(bootstrap_method** bm)
{
	*bm = new bootstrap_method;
	m_Ptrs.push_back(new Pointer('o', 0, *bm));
	memset(bm, 0x0, sizeof(bootstrap_method));

	memcpy(&(*bm)->bootstrap_method_ref, where_, sizeof((*bm)->bootstrap_method_ref));
	where_ += sizeof((*bm)->bootstrap_method_ref);

	memcpy(&(*bm)->num_bootstrap_arguments, where_, sizeof((*bm)->num_bootstrap_arguments));
	(*bm)->num_bootstrap_arguments = swapu2((*bm)->num_bootstrap_arguments);
	where_ += sizeof((*bm)->num_bootstrap_arguments);
	if ((*bm)->num_bootstrap_arguments == 0)return;

	(*bm)->bootstrap_arguments = new u2[(*bm)->num_bootstrap_arguments];
	m_Ptrs.push_back(new Pointer('[', (*bm)->num_bootstrap_arguments, (*bm)->bootstrap_arguments));
	memcpy((*bm)->bootstrap_arguments, where_, sizeof(u2) * (*bm)->num_bootstrap_arguments);
}

void CClassFileParser::read_verification_type_info(verification_type_info** pinfo)
{
	/*
	* this function just calculate the pointer offset, no data will be read.
	* because these data is not usefull here, you can fix it
	*/
	*pinfo = new verification_type_info;
	u1 tag = *(where_);
	switch (tag)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	{
		where_ += sizeof(u1);
		break;
	}
	case 7:
	case 8:
	{
		where_ += sizeof(Object_variable_info);
		break;
	}
	}
}

void CClassFileParser::delete_array(void** array, int n)
{
	if (array)
	{
		if (n > 0)
		{
			for (int i = 0; i < n; i++)
			{
				delete array[n];
			}
		}
	}
}
