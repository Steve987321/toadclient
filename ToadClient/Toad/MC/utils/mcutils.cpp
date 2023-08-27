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
        std::string res = name;
        env->ReleaseStringUTFChars(jStr, name);
        return res;
    }

    std::unordered_map<mapping, jmethodID> cached_mids;
    std::unordered_map<mappingFields, jfieldID> cached_fids;

    jmethodID get_mid(jclass cls, mapping name, JNIEnv* env)
    {
#ifdef ENABLE_LOGGING
        if (!mappings::methodnames.contains(name))
        {
            LOGERROR("mapping name was not found with mapping name: {}, and index {}", mappings::findName(name), static_cast<int>(name));
            SLEEP(300);
            return nullptr;
        }
#endif
        if (auto it = cached_mids.find(name); it != cached_mids.end())
        {
            //std::cout << "found\n";
            return it->second;
        }

        auto mid = env->GetMethodID(cls, mappings::findName(name), mappings::findSig(name));
#ifdef ENABLE_LOGGING
        if (!mid)
        {
            LOGERROR("methodId is null with mapping name: {}, and index {}", mappings::findName(name), static_cast<int>(name));
            SLEEP(300);
        }
#endif

        cached_mids.insert({name, mid});
        std::cout << cached_mids.size() << std::endl;

        return mid; 
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
#endif

        if (auto it = cached_mids.find(name); it != cached_mids.end())
        {
            return it->second;
        }
       
        auto smId = env->GetStaticMethodID(cls, mappings::findName(name), mappings::findSig(name));

#ifdef ENABLE_LOGGING
        if (!smId)
        {
            LOGERROR("static methodId is null with mapping name: {} and index: {}", mappings::findName(name), static_cast<int>(name));
            SLEEP(300);
            return nullptr;
        }
#endif
        cached_mids.insert({ name, smId });

        return smId;
    }

    jfieldID get_static_fid(jclass cls, mappingFields name, JNIEnv* env)
	{
#ifdef ENABLE_LOGGING
        if (!mappings::fieldnames.contains(name))
        {
            LOGERROR("static field mapping name was not found with mapping name: {}, and index {}", mappings::findNameField(name), static_cast<int>(name));
            SLEEP(300);
        }
#endif

        if (auto it = cached_fids.find(name); it != cached_fids.end())
        {
            return it->second;
        }

        auto sfId = env->GetStaticFieldID(cls, mappings::findNameField(name), mappings::findSigField(name));
#ifdef ENABLE_LOGGING
        if (!sfId)
        {
            LOGERROR("static fieldId is null with mapping name: {}, and index: {}", mappings::findNameField(name), static_cast<int>(name));
            SLEEP(300);
            return nullptr;
        }
#endif
        cached_fids.insert({name, sfId});
        return sfId;
	}
    
    jfieldID get_fid(jclass cls, mappingFields name, JNIEnv* env)
	{
#ifdef ENABLE_LOGGING
        if (!mappings::fieldnames.contains(name))
        {
            LOGERROR("field mapping name was not found with mapping name: {}, and index {}", mappings::findNameField(name), static_cast<int>(name));
            SLEEP(300);
        }
#endif
        if (auto it = cached_fids.find(name); it != cached_fids.end())
        {
            return it->second;
        }

        auto fId = env->GetFieldID(cls, mappings::findNameField(name), mappings::findSigField(name));
#ifdef ENABLE_LOGGING
        if (!fId)
        {
            LOGERROR("fieldId is null with mapping name: {}, and index: {}", mappings::findNameField(name), static_cast<int>(name));
            SLEEP(300);
            return nullptr;
        }
#endif
        cached_fids.insert({ name, fId });

        return fId;
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

        static auto xposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3X), "()D");
        if (!xposid) return {-1, -1, -1};

        static auto yposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3Y), "()D");
        static auto zposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3Z), "()D");

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

        static auto xposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3IX), "()I");
        if (!xposid) return {-1, 0, 0};

        static auto yposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3IY), "()I");
        static auto zposid = env->GetMethodID(posclass, mappings::findName(mapping::Vec3IZ), "()I");

        env->DeleteLocalRef(posclass);

        return {
	        (float)env->CallIntMethod(vecObj, xposid), 
            (float)env->CallIntMethod(vecObj, yposid),
            (float)env->CallIntMethod(vecObj, zposid)
        };
    }

    void loop_through_class(const jclass klass, JNIEnv* env)
    {
        for (auto i = 0; i < jvmfunc::oJVM_GetClassMethodsCount(env, klass); i++)
        {
            LOGDEBUG("name: {}, sig: {} args size: {}", jvmfunc::oJVM_GetMethodIxNameUTF(env, klass, i), jvmfunc::oJVM_GetMethodIxSignatureUTF(env, klass, i), jvmfunc::oJVM_GetMethodIxArgsSize(env, klass, i));
		}

    }
}
