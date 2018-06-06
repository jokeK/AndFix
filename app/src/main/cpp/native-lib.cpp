#include <jni.h>
#include <string>
#include "dalvik.h"
#include "art_method.h"
//#include "art_7_0.h"
//两个头文件不能共存 写两个CPP

typedef Object* (*FindObject)(void* thread,jobject jobj);
typedef void *(*FindThread)();
FindObject findObject;
FindThread findThread;
extern "C"
JNIEXPORT void JNICALL
Java_com_mik_andfix_DexManager_replaceMethod(
        JNIEnv *env, jobject instance,jint sdk_version,jobject wrongMethod, jobject rightMethod) {
    //找到虚拟机对应的Method结构体指针
    Method* wrong = (Method *)(env->FromReflectedMethod(wrongMethod));
    Method* right = (Method *)(env->FromReflectedMethod(rightMethod));
   //使用hook 得到两个方法，一个是得到java Class对象对应NDK中的Object对象
    //一个是得到当前线程
    void *dvm_hand = dlopen("libdvm.so",RTLD_NOW);
    //hook得到两个libdvm.so中的方法
    //第一个方法得到NDK对应的ClassObject
    //第二个得到当前线程
    //sdk  10   以后方法名变了
    findObject = (FindObject)(dlsym(dvm_hand, sdk_version > 10 ? "_Z20dvmDecodeIndirectRefP6ThreadP8_jobject" : "dvmDecodeIndirectRef"));
    findThread = (FindThread)(dlsym(dvm_hand, sdk_version > 10 ? "_Z13dvmThreadSelfv" :"dvmThreadSelf"));

    //通过Method.class类中的getDeclaringClass方法
    //拿到java method对应的ClassObject对象
    jclass  methodClz = env->FindClass("java/lang/reflect/Method");
    jmethodID rightMethodID = env->GetMethodID(methodClz,"getDeclaringClass","()Ljava/lang/Class");
    jobject ndkObiect = env->CallObjectMethod(rightMethod,rightMethodID);
    //拿到对应ndk中的ClassObject对象
    ClassObject *firstFiled = (ClassObject *)(findObject(findThread(), ndkObiect));

    //修改right对应的classObject的加载状态ClassStatus为CLASS_INITIALIZED
    firstFiled->status = CLASS_INITIALIZED;

    //把错误方法的信息全部指向正确的方法
    //wrong->accessFlags = ACC_PUBLIC;
    wrong->methodIndex = right->methodIndex;
    wrong->jniArgInfo = right->jniArgInfo;
    wrong->registersSize = right->registersSize;
    wrong->outsSize = right->outsSize;
    //方法参数原型
    wrong->prototype = right->prototype;
    wrong->insns = right->insns;
    wrong->nativeFunc = right->nativeFunc;

}

JNIEXPORT void JNICALL
Java_com_mik_andfix_DexManager_replaceArt(JNIEnv *env, jobject instance, jobject wrongMethod,
                                          jobject rightMethod) {
    art::mirror::ArtMethod *wrong = (art::mirror::ArtMethod *)(env->FromReflectedMethod(wrongMethod));
    art::mirror::ArtMethod *right = (art::mirror::ArtMethod *)(env->FromReflectedMethod(rightMethod));

    wrong->declaring_class_ = right->declaring_class_;
    wrong->dex_cache_resolved_methods_ = right->dex_cache_resolved_methods_;
    wrong->dex_cache_resolved_types_ = right->dex_cache_resolved_types_;
    wrong->dex_code_item_offset_ = right->dex_code_item_offset_;
    wrong->method_index_ = right->method_index_;
    wrong->dex_method_index_ = right->dex_method_index_;

    wrong->ptr_sized_fields_.entry_point_from_interpreter_ = right->ptr_sized_fields_
            .entry_point_from_interpreter_;
    wrong->ptr_sized_fields_.entry_point_from_jni_ = right->ptr_sized_fields_
            .entry_point_from_jni_;
    //机器码模式
    wrong->ptr_sized_fields_.entry_point_from_quick_compiled_code_ = right->ptr_sized_fields_
            .entry_point_from_quick_compiled_code_;

}

JNIEXPORT void JNICALL
Java_com_mik_andfix_DexManager_replaceArtN(JNIEnv *env, jobject instance, jobject wrongMethod,
                                             jobject rightMethod) {
    art::mirror::ArtMethod *wrong = (art::mirror::ArtMethod *)(env->FromReflectedMethod(wrongMethod));
    art::mirror::ArtMethod *right = (art::mirror::ArtMethod *)(env->FromReflectedMethod(rightMethod));

    wrong->declaring_class_ = right->declaring_class_;
    wrong->dex_code_item_offset_ = right->dex_code_item_offset_;
    wrong->method_index_ = right->method_index_;
    wrong->dex_method_index_ = right->dex_method_index_;

    wrong->ptr_sized_fields_.entry_point_from_jni_ = right->ptr_sized_fields_
            .entry_point_from_jni_;
    //机器码模式
    wrong->ptr_sized_fields_.entry_point_from_quick_compiled_code_ = right->ptr_sized_fields_
            .entry_point_from_quick_compiled_code_;

    wrong->ptr_sized_fields_.dex_cache_resolved_types_ = right->ptr_sized_fields_.dex_cache_resolved_types_;
    wrong->ptr_sized_fields_.dex_cache_resolved_methods_ = right->ptr_sized_fields_.dex_cache_resolved_methods_;
    wrong->hotness_count_ = right->hotness_count_;

}