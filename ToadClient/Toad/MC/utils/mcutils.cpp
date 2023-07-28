#include "pch.h"
#include "mcutils.h"

#include "Toad/Toad.h"

namespace toadll
{

    jclass findclass(const char* clsName, JNIEnv* env)
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

        env->DeleteLocalRef(array_elements);
        env->DeleteLocalRef(thread_clazz);
        env->DeleteLocalRef(thread);
        env->DeleteLocalRef(threadgroup_clazz);
        env->DeleteLocalRef(arrayD);
        env->DeleteLocalRef(threadgroup_obj);

        return env->FindClass(clsName);
    }

    std::string jstring2string(jstring jStr, JNIEnv* env) {
        if (!jStr)
            return "";
        auto name = env->GetStringUTFChars(jStr, nullptr);
        env->ReleaseStringUTFChars(jStr, name);
        return std::string(name);
    }

    jmethodID get_mid(jclass cls, mapping name, JNIEnv* env)
    {
#ifdef ENABLE_LOGGING
        if (!mappings::methodnames.contains(name))
        {
            LOGERROR("mapping name was not found with mapping name: {}, and index {}", mappings::findName(name), static_cast<int>(name));
            SLEEP(300);
            return nullptr;
        }
        auto mid = env->GetMethodID(cls, mappings::findName(name), mappings::findSig(name));
        if (!mid)
        {
            LOGERROR("methodId is null with mapping name: {}, and index {}", mappings::findName(name), static_cast<int>(name));
            SLEEP(300);
        }
        return mid; 
#else
        return env->GetMethodID(cls, mappings::findName(name), mappings::findSig(name));
#endif
    }

    jmethodID get_mid(jobject obj, mapping name, JNIEnv* env)
    {
        auto objKlass = env->GetObjectClass(obj);
        if (!objKlass)
        {
#ifdef ENABLE_LOGGING
            LOGERROR("getting object class for methodid returned null with mapping name: {}, and index {}", mappings::findName(name), static_cast<int>(name));
            SLEEP(300);
#endif
            return nullptr;
        }
        auto mid = get_mid(objKlass, name, env);
        env->DeleteLocalRef(objKlass);
        return mid;
    }

    jmethodID get_static_mid(jclass cls, mapping name, JNIEnv* env)
    {
#ifdef ENABLE_LOGGING
        if (!mappings::methodnames.contains(name))
        {
            LOGERROR("mapping name was not found with mapping name: {}, and index {}", mappings::findName(name), static_cast<int>(name));
            SLEEP(300);
        }
        auto smId = env->GetStaticMethodID(cls, mappings::findName(name), mappings::findSig(name));
        if (!smId)
        {
            LOGERROR("static methodId is null with mapping name: {} and index: {}", mappings::findName(name), static_cast<int>(name));
            SLEEP(300);
            return nullptr;
        }
        return smId;
#else
        return env->GetStaticMethodID(cls, mappings::findName(name), mappings::findSig(name));
#endif
    }

    jfieldID get_static_fid(jclass cls, mappingFields name, JNIEnv* env)
	{
#ifdef ENABLE_LOGGING
        if (!mappings::fieldnames.contains(name))
        {
            LOGERROR("static field mapping name was not found with mapping name: {}, and index {}", mappings::findNameField(name), static_cast<int>(name));
            SLEEP(300);
        }
        auto sfId = env->GetStaticFieldID(cls, mappings::findNameField(name), mappings::findSigField(name));
        if (!sfId)
        {
            LOGERROR("static fieldId is null with mapping name: {}, and index: {}", mappings::findNameField(name), static_cast<int>(name));
            SLEEP(300);
            return nullptr;
        }
        return sfId;
#else
        return env->GetStaticFieldID(cls, mappings::findNameField(name), mappings::findSigField(name));
#endif
	}
    
    jfieldID get_fid(jclass cls, mappingFields name, JNIEnv* env)
	{
#ifdef ENABLE_LOGGING
        if (!mappings::fieldnames.contains(name))
        {
            LOGERROR("field mapping name was not found with mapping name: {}, and index {}", mappings::findNameField(name), static_cast<int>(name));
            SLEEP(300);
        }
        auto fId = env->GetFieldID(cls, mappings::findNameField(name), mappings::findSigField(name));
        if (!fId)
        {
            LOGERROR("fieldId is null with mapping name: {}, and index: {}", mappings::findNameField(name), static_cast<int>(name));
            SLEEP(300);
            return nullptr;
        }
        return fId;
#else
        return env->GetFieldID(cls, mappings::findNameField(name), mappings::findSigField(name));
#endif
	}

	jfieldID get_fid(jobject obj, mappingFields name, JNIEnv* env)
	{
        auto objKlass = env->GetObjectClass(obj);
        if (!objKlass)
        {
#ifdef ENABLE_LOGGING
            LOGERROR("getting object class for fieldid returned null with mapping name: {}, and index: {}", mappings::findNameField(name), static_cast<int>(name));
            SLEEP(300);
#endif
        	return nullptr;
        }
        auto res = get_fid(objKlass, name, env);
        env->DeleteLocalRef(objKlass);
        return res;
	}
    

    Vec3 to_vec3(jobject vecObj, JNIEnv* env)
    {
        auto posclass = env->GetObjectClass(vecObj);

        auto xposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3X), "()D");
        if (!xposid) return {-1, -1, -1};

        auto yposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3Y), "()D");
        auto zposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3Z), "()D");

        env->DeleteLocalRef(posclass);

        return {
	        (float)env->CallDoubleMethod(vecObj, xposid), 
            (float)env->CallDoubleMethod(vecObj, yposid),
            (float)env->CallDoubleMethod(vecObj, zposid)
        };
    }

	Vec3 to_vec3i(jobject vecObj, JNIEnv* env)
    {
        auto posclass = env->GetObjectClass(vecObj);

        auto xposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3IX), "()I");
        if (!xposid) return {-1, 0, 0};

        auto yposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3IY), "()I");
        auto zposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3IZ), "()I");

        env->DeleteLocalRef(posclass);

        return {
	        (float)env->CallIntMethod(vecObj, xposid), 
            (float)env->CallIntMethod(vecObj, yposid),
            (float)env->CallIntMethod(vecObj, zposid)
        };
    }

    std::pair<float, float> toadll::get_angles(const Vec3& pos1, const Vec3& pos2)
    {
        float d_x = pos2.x - pos1.x;
        float d_y = pos2.y - pos1.y;
        float d_z = pos2.z - pos1.z;

        float hypothenuse = sqrt(d_x * d_x + d_z * d_z);
        float yaw = atan2(d_z, d_x) * 180.f / g_PI - 90.f;
        float pitch = -atan2(d_y, hypothenuse) * 180 / g_PI;

        return std::make_pair(yaw, pitch);
    }

    float toadll::wrap_to_180(float value)
    {
        float res = std::fmod(value + 180, 360);
        if (res < 0)
            res += 360;
        return res - 180;
    }

    void loop_through_class(const jclass klass, JNIEnv* env)
    {
        for (auto i = 0; i < jvmfunc::oJVM_GetClassMethodsCount(env, klass); i++)
        {
            LOGDEBUG("name: {}, sig: {} args size: %d", jvmfunc::oJVM_GetMethodIxNameUTF(env, klass, i), jvmfunc::oJVM_GetMethodIxSignatureUTF(env, klass, i), jvmfunc::oJVM_GetMethodIxArgsSize(env, klass, i));
		}

    }
}
