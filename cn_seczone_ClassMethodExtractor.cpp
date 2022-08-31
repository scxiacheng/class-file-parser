#include "cn_seczone_ClassMethodExtractor.h"
#include "ClassFileParser.h"
#include <string>
#include <vector>

struct ClassMethod_cls
{
	jclass cls;
	jmethodID setClazz;
	jmethodID setName;
	jmethodID setDesc;
};

struct MethodInfo
{
	std::string name;
	std::string desc;
};

static void release_method_info_buffer(std::vector<MethodInfo*>& infos)
{
	std::vector<MethodInfo*>::iterator it = infos.begin();
	for (it; it != infos.end(); it++)
	{
		delete (*it);
	}
}


static void buildClassMethod_cls(JNIEnv* env, ClassMethod_cls* pCls)
{
	pCls->cls = env->FindClass("cn/seczone/data/SimpleMethod");
	if (NULL == pCls->cls)
	{
		printf("[NATIVE-INFO] pCls->cls == NULL\n");
		return;
	}

	pCls->setClazz = env->GetMethodID(pCls->cls, "setClazz", "(Ljava/lang/String;)V");
	if (NULL == pCls->setClazz)
	{
		printf("[NATIVE-INFO] pCls->setClazz == NULL\n");
		return;
	}
	pCls->setName = env->GetMethodID(pCls->cls, "setName", "(Ljava/lang/String;)V");
	if (NULL == pCls->setName)
	{
		printf("[NATIVE-INFO] pCls->setName == NULL\n");
		return;
	}
	pCls->setDesc = env->GetMethodID(pCls->cls, "setDesc", "(Ljava/lang/String;)V");
	if (NULL == pCls->setDesc)
	{
		printf("[NATIVE-INFO] pCls->setDesc == NULL\n");
		return;
	}
}

static char* get_text(cp_info** ci, u2 index)
{
	CONSTANT_Utf8_info* utf8 = (CONSTANT_Utf8_info*)ci[index];
	return (char*)utf8->bytes;
}

static char* get_this_class(cp_info** ci, u2 this_class)
{
	if (this_class == 0)
	{
		return (char*)"NO NAME";
	}

	u2 index = this_class - 1;
	CONSTANT_Class_info* p = (CONSTANT_Class_info*)ci[index];
	index = p->name_index;
	return get_text(ci, index - 1);
}

static size_t getMethodInfo(ClassFile* pClassFile, std::vector<MethodInfo*>& infos)
{
	cp_info** ci = pClassFile->constant_pool;
	if (ci == NULL)
	{
		return size_t(0);
	}

	for (int i = 0; i < pClassFile->methods_count; i++)
	{
		method_info* p = pClassFile->methods[i];
		char* method_name = get_text(ci, p->name_index - 1);
		char* desc_str = get_text(ci, p->descriptor_index - 1);
		MethodInfo* mi = new MethodInfo;
		mi->name = method_name;
		mi->desc = desc_str;
		infos.push_back(mi);
	}
	return infos.size();
}


JNIEXPORT jobject JNICALL Java_cn_seczone_ClassMethodExtractor_nativeParseClassFile(JNIEnv* env, jobject thiz, jstring classFile)
{
	const char* classFileName = env->GetStringUTFChars(classFile, NULL);
	printf("[NATIVE-INFO] process %s\n", classFileName);

	CClassFileParser* pClassFileParser = new CClassFileParser(classFileName);
	if (pClassFileParser == NULL)
	{
		printf("[NATIVE-INFO] construct CClassFileParser failed\n");
		return NULL;
	}

	printf("[NATIVE-INFO] begin parser class file.\n");
	ClassFile* pClassFile = pClassFileParser->Parse();
	if (pClassFile->methods_count == 0)
	{
		pClassFileParser->Release();
		printf("[NATIVE-INFO] method_count == 0\n");
		return NULL;
	}
	printf("[NATIVE-INFO] finish parser class file.\n");

	char* clazzName = get_this_class(pClassFile->constant_pool, pClassFile->this_class);
	printf("[NATIVE-INFO] clazzName=%s\n", clazzName);
	std::string strClassName = clazzName;

	std::vector<MethodInfo*> methods;
	if (0 == getMethodInfo(pClassFile, methods))
	{
		pClassFileParser->Release();
		printf("[NATIVE-INFO] method_count == 0\n");
		return NULL;
	}
	// if everything goes well, then release buffer
	printf("[NATIVE-INFO] pClassFileParser->Release()\n");
	pClassFileParser->Release();

	// convert native data to java data
	jclass arrayList = env->FindClass("java/util/ArrayList");
	if (NULL == arrayList)
	{
		printf("[NATIVE-INFO] can not find class named java/lang/ArrayList");
		return NULL;
	}
	
	jmethodID init = env->GetMethodID(arrayList, "<init>", "()V");
	if (NULL == init)
	{
		printf("[NATIVE-INFO] can not find method named init.\n");
		return NULL;
	}
	jmethodID add = env->GetMethodID(arrayList, "add", "(Ljava/lang/Object;)Z");
	if (add == init)
	{
		printf("[NATIVE-INFO] can not find method named add.\n");
		return NULL;
	}

	jobject arryListObject = env->NewObject(arrayList, init);
	ClassMethod_cls cls;
	buildClassMethod_cls(env, &cls);
	if (NULL == cls.cls || NULL == cls.setClazz || NULL == cls.setDesc || NULL == cls.setName)
	{
		printf("[NATIVE-INFO] ClassMethod_cls contains NULL.\n");
		return NULL;
	}

	printf("[NATIVE-INFO] translating c++ data type to java data type\n");
	std::vector<MethodInfo*>::iterator it = methods.begin();
	for (it; it != methods.end(); it++)
	{
		jobject simpleMethod = env->AllocObject(cls.cls);
		if (NULL == simpleMethod)
		{
			printf("[NATIVE-INFO] simpleMethod==NULL\n");
			continue;
		}
		printf("[NATIVE-INFO] method: class_name=>%s, method_name=>%s, method_desc=>%s\n", 
			strClassName.c_str(),
			(*it)->name.c_str(), 
			(*it)->desc.c_str()
		);
		env->CallVoidMethod(simpleMethod, cls.setClazz, strClassName.c_str());
		env->CallVoidMethod(simpleMethod, cls.setName, (*it)->name.c_str());
		env->CallVoidMethod(simpleMethod, cls.setDesc, (*it)->desc.c_str());
		env->CallBooleanMethod(arryListObject, add, simpleMethod);
	}

	printf("[NATIVE-INFO] release_method_info_buffer 1\n");
	release_method_info_buffer(methods);
	printf("[NATIVE-INFO] release_method_info_buffer 2\n");
	return arryListObject;
}
