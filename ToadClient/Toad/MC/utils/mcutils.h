#pragma once

#include <gl/GL.h>

#include "Toad/Types.h"

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
    jclass findclass(const char* clsName);

    std::string jstring2string(const jstring& jStr);

    // get jmethodID from obj or class from the given mapping
    jmethodID get_mid(jclass cls, mapping name);
    jmethodID get_mid(jobject obj, mapping name);
    jmethodID get_static_mid(jclass cls, mapping name);

    // get jfieldId from obj or class from the given mapping
    jfieldID get_fid(jclass cls, mappingFields name);
    jfieldID get_fid(jobject obj, mappingFields name);
    jfieldID get_static_fid(jclass cls, mappingFields name);

	// minecraft Vec3 object to vec3
	vec3 to_vec3(jobject vecObj);
	vec3 to_vec3i(jobject vecObj);

    std::pair<float, float> get_angles(const vec3& pos1, const vec3& pos2);
    float wrap_to_180(float value);

    bool WorldToScreen(const vec3& source, const vec3& target, const vec2& viewAngles, float fov, vec2& screenpos);

    void loop_through_class(const jclass klass);

}
