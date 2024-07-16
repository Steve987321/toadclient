#pragma once

#include <gl/GL.h>

#include "Toad/types.h"

namespace toadll
{
    // jvm functions
    namespace jvmfunc
    {
        typedef jint
        (*hJNI_GetCreatedJavaVMs)(JavaVM** vmBuf, jsize bufLen, jsize* nVMs);
        inline hJNI_GetCreatedJavaVMs oJNI_GetCreatedJavaVMs;

        /*
         * Returns the number of *declared* fields or methods.
         */
        typedef jint
        (*hJVM_GetClassFieldsCount)(JNIEnv* env, jclass cb);
        inline hJVM_GetClassFieldsCount oJVM_GetClassFieldsCount;

        typedef jint
        (*hJVM_GetClassMethodsCount)(JNIEnv* env, jclass cb);
        inline hJVM_GetClassMethodsCount oJVM_GetClassMethodsCount;

        /*
         * Returns the name of a given method in UTF format.
         * The result remains valid until JVM_ReleaseUTF is called.
         *
         * The caller must treat the string as a constant and not modify it
         * in any way.
         */
        typedef const char*
            (*hJVM_GetMethodIxNameUTF)(JNIEnv* env, jclass klass, jint index);
        inline hJVM_GetMethodIxNameUTF oJVM_GetMethodIxNameUTF;

        /*
         * Returns the signature of the method referred to at a given constant pool
         * index.
         *
         * The result is in UTF format and remains valid until JVM_ReleaseUTF
         * is called.
         *
         * The caller must treat the string as a constant and not modify it
         * in any way.
         */
        typedef const char*
            (*hJVM_GetMethodIxSignatureUTF)(JNIEnv* env, jclass cb, jint index);
        inline hJVM_GetMethodIxSignatureUTF oJVM_GetMethodIxSignatureUTF;

        typedef jobjectArray
        (*hJVM_GetClassDeclaredFields)(JNIEnv* env, jclass ofClass, jboolean publicOnly);
        inline hJVM_GetClassDeclaredFields oJVM_GetClassDeclaredFields;

        typedef jint
        (*hJVM_GetArrayLength)(JNIEnv* env, jobject arr);
        inline hJVM_GetArrayLength oJVM_GetArrayLength;

        typedef jobject
        (*hJVM_GetArrayElement)(JNIEnv* env, jobject arr, jint index);
        inline hJVM_GetArrayElement oJVM_GetArrayElement;

        typedef jint
        (*hJVM_GetMethodIxArgsSize)(JNIEnv* env, jclass cb, int index);
        inline hJVM_GetMethodIxArgsSize oJVM_GetMethodIxArgsSize;
    }

    // function to find classes on (any) minecraft client
	// https://www.unknowncheats.me/forum/minecraft/482232-lunar-client-class.html
    jclass findclass(const char* clsName, JNIEnv* env);

    std::string jstring2string(jstring jStr, JNIEnv* env);

    // get jmethodID from obj or class from the given mapping
    jmethodID get_mid(jclass cls, mapping name, JNIEnv* env);
    jmethodID get_mid(jobject obj, mapping name, JNIEnv* env);
    jmethodID get_static_mid(jclass cls, mapping name, JNIEnv* env);

    // get jfieldId from obj or class from the given mapping
    jfieldID get_fid(jclass cls, mappingFields name, JNIEnv* env);
    jfieldID get_fid(jobject obj, mappingFields name, JNIEnv* env);
    jfieldID get_static_fid(jclass cls, mappingFields name, JNIEnv* env);

	// minecraft Vec3 object to vec3
	Vec3 to_vec3(jobject vecObj, JNIEnv* env);
	Vec3 to_vec3i(jobject vecObj, JNIEnv* env);

    //vec3 get_closest_point()

    void loop_through_class(const jclass klass, JNIEnv* env);

}
