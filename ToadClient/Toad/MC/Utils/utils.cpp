#include "pch.h"
#include "utils.h"

#include "Toad/Toad.h"

namespace toadll
{
    jclass findclass(const char* clsName)
    {
        jclass thread_clazz = env->FindClass("java/lang/Thread");
        jmethodID curthread_mid = env->GetStaticMethodID(thread_clazz, "currentThread", "()Ljava/lang/Thread;");
        jobject thread = env->CallStaticObjectMethod(thread_clazz, curthread_mid);
        jmethodID threadgroup_mid = env->GetMethodID(thread_clazz, "getThreadGroup", "()Ljava/lang/ThreadGroup;");
        jclass threadgroup_clazz = env->FindClass("java/lang/ThreadGroup");
        jobject threadgroup_obj = env->CallObjectMethod(thread, threadgroup_mid);
        jmethodID groupactivecount_mid = env->GetMethodID(threadgroup_clazz, "activeCount", "()I");
        jfieldID count_fid = env->GetFieldID(threadgroup_clazz, "nthreads", "I");
        jint activeCount = env->GetIntField(threadgroup_obj, count_fid);
        jobjectArray arrayD = env->NewObjectArray(activeCount, thread_clazz, NULL);
        jmethodID enumerate_mid = env->GetMethodID(threadgroup_clazz, "enumerate", "([Ljava/lang/Thread;)I");
        jint enumerate = env->CallIntMethod(threadgroup_obj, enumerate_mid, arrayD);
        jmethodID mid_getname = env->GetMethodID(thread_clazz, "getName", "()Ljava/lang/String;");
        jobject array_elements = env->GetObjectArrayElement(arrayD, 0);
        jmethodID threadclassloader = env->GetMethodID(thread_clazz, "getContextClassLoader", "()Ljava/lang/ClassLoader;");
        if (threadclassloader != 0)
        {
            auto class_loader = env->CallObjectMethod(array_elements, threadclassloader);
            jclass launch_clazz = env->FindClass("net/minecraft/launchwrapper/Launch");

            auto find_class_id = env->GetMethodID(env->GetObjectClass(class_loader), "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");

            env->DeleteLocalRef(launch_clazz);
            jstring name = env->NewStringUTF(clsName);

            env->DeleteLocalRef(array_elements);
            env->DeleteLocalRef(thread_clazz);
            env->DeleteLocalRef(thread);
            env->DeleteLocalRef(threadgroup_clazz);
            env->DeleteLocalRef(threadgroup_obj);
            env->DeleteLocalRef(arrayD);

            return jclass(env->CallObjectMethod(class_loader, find_class_id, name));
        }
        else
        {
            env->DeleteLocalRef(array_elements);
            env->DeleteLocalRef(thread_clazz);
            env->DeleteLocalRef(thread);
            env->DeleteLocalRef(threadgroup_clazz);
            env->DeleteLocalRef(arrayD);
            env->DeleteLocalRef(threadgroup_obj);

            return env->FindClass(clsName);
        }
    }

    std::string jstring2string(const jstring& jStr) {
        if (!jStr)
            return "";

        const jclass stringClass = env->GetObjectClass(jStr);
        const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
        const auto stringJbytes = (jbyteArray)env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

        auto length = (size_t)env->GetArrayLength(stringJbytes);
        jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

        auto ret = std::string(reinterpret_cast<char*>(pBytes), length);
        env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

        env->DeleteLocalRef(stringJbytes);
        env->DeleteLocalRef(stringClass);
        return ret;
    }

    jmethodID get_mid(const jclass& cls, mapping name)
    {
        return env->GetMethodID(cls, mappings::findName(name), mappings::findSig(name));
    }

    jmethodID get_mid(const jobject& obj, mapping name)
    {
        return get_mid(env->GetObjectClass(obj), name);
    }

    jmethodID get_static_mid(const jclass& cls, const mapping& name)
    {
        return env->GetStaticMethodID(cls, mappings::findName(name), mappings::findSig(name));
    }  

    vec3 to_vec3(const jobject& vecObj)
    {
        auto posclass = env->GetObjectClass(vecObj);

        auto xposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3X), "()D");
        if (!xposid) return {-1, 0, 0};

        auto yposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3Y), "()D");
        auto zposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3Z), "()D");

        env->DeleteLocalRef(posclass);

        return vec3(
            (float)env->CallDoubleMethod(vecObj, xposid), 
            (float)env->CallDoubleMethod(vecObj, yposid),
            (float)env->CallDoubleMethod(vecObj, zposid)
        );
    }

    std::pair<float, float> toadll::get_angles(const vec3& pos1, const vec3& pos2)
    {
        float d_x = pos2.x - pos1.x;
        float d_y = pos2.y - pos1.y;
        float d_z = pos2.z - pos1.z;

        float hypothenuse = sqrt(d_x * d_x + d_z * d_z);
        float yaw = atan2(d_z, d_x) * 180.f / PI - 90.f;
        float pitch = -atan2(d_y, hypothenuse) * 180 / PI;

        return std::make_pair(yaw, pitch);
    }

    float toadll::wrap_to_180(float value)
    {
        float res = std::fmod(value + 180, 360);
        if (res < 0)
            res += 360;
        return res - 180;
    }
}
