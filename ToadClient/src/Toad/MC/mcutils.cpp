#include "pch.h"
#include "mcutils.h"

#include "Toad/Toad.h"

namespace toadll
{
    jclass findclass(const char* clsName, JNIEnv* env)
    {
        jclass thread_clazz = env->FindClass("java/lang/Thread");
        static jmethodID curthread_mid = env->GetStaticMethodID(thread_clazz, "currentThread", "()Ljava/lang/Thread;");
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
            jclass class_loader_class = env->GetObjectClass(class_loader);
            jmethodID find_class_id = env->GetMethodID(class_loader_class, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");

            env->DeleteLocalRef(launch_clazz);
            jstring name = env->NewStringUTF(clsName);

            jclass res = (jclass)env->CallObjectMethod(class_loader, find_class_id, name);

            env->DeleteLocalRef(name);
            env->DeleteLocalRef(class_loader_class);
            env->DeleteLocalRef(array_elements);
            env->DeleteLocalRef(thread_clazz);
            env->DeleteLocalRef(thread);
            env->DeleteLocalRef(threadgroup_clazz);
            env->DeleteLocalRef(threadgroup_obj);
            env->DeleteLocalRef(arrayD);

            return res;
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

    static std::unordered_map<mapping, jmethodID> cached_mids;
    static std::unordered_map<mappingFields, jfieldID> cached_fids;

    jmethodID get_mid(jclass cls, mapping mcmapping, JNIEnv* env)
    {
#ifdef ENABLE_LOGGING
        if (!mappings::methods.contains(mcmapping))
        {
            LOGERROR("mapping name was not found with mapping index {}", static_cast<int>(mcmapping));
            SLEEP(300);
            return nullptr;
        }
#endif
        if (auto it = cached_mids.find(mcmapping); it != cached_mids.end())
        {
            return it->second;
        }
        
		auto it = mappings::methods.find(mcmapping);
		const mappings::MCMap& mc_map = it->second;

        auto mid = env->GetMethodID(cls, mc_map.name.c_str(), mc_map.sig.c_str());
#ifdef ENABLE_LOGGING
        if (!mid)
        {
            LOGERROR("methodId is null with mapping name: {}, and index {}", mc_map.name, static_cast<int>(mcmapping));
            SLEEP(300);
        }
#endif
        cached_mids.insert({mcmapping, mid});

        return mid; 
    }

    jmethodID get_mid(jobject obj, mapping name, JNIEnv* env)
    {
        auto objKlass = env->GetObjectClass(obj);
        if (!objKlass)
        {
#ifdef ENABLE_LOGGING
            LOGERROR("getting object class for methodid returned null with index {}", static_cast<int>(name));
            SLEEP(300);
#endif
            return nullptr;
        }
        auto mid = get_mid(objKlass, name, env);
        env->DeleteLocalRef(objKlass);
        return mid;
    }

    jmethodID get_static_mid(jclass cls, mapping mcmapping, JNIEnv* env)
    {
#ifdef ENABLE_LOGGING
        if (!mappings::methods.contains(mcmapping))
        {
            LOGERROR("mapping name was not found with index {}", static_cast<int>(mcmapping));
            SLEEP(300);
            return nullptr;
        }
#endif

        if (auto it = cached_mids.find(mcmapping); it != cached_mids.end())
        {
            return it->second;
        }

		auto it = mappings::methods.find(mcmapping);
		const mappings::MCMap& mc_map = it->second;

        auto smid = env->GetStaticMethodID(cls, mc_map.name.c_str(), mc_map.sig.c_str());

#ifdef ENABLE_LOGGING
        if (!smid)
        {
            LOGERROR("static methodId is null with mapping name: {} and index: {}", mc_map.name, static_cast<int>(mcmapping));
            SLEEP(300);
            return nullptr;
        }
#endif
        cached_mids.insert({ mcmapping, smid });

        return smid;
    }

    jfieldID get_static_fid(jclass cls, mappingFields mcmapping, JNIEnv* env)
	{
#ifdef ENABLE_LOGGING
        if (!mappings::fields.contains(mcmapping))
        {
            LOGERROR("static field mapping name was not found with index {}", static_cast<int>(mcmapping));
            SLEEP(300);
        }
#endif

        if (auto it = cached_fids.find(mcmapping); it != cached_fids.end())
        {
            return it->second;
        }

		auto it = mappings::fields.find(mcmapping);
		const mappings::MCMap& mc_map = it->second;

        auto sfId = env->GetStaticFieldID(cls, mc_map.name.c_str(), mc_map.sig.c_str());
#ifdef ENABLE_LOGGING
        if (!sfId)
        {
            LOGERROR("static fieldId is null with mapping name: {}, and index: {}", mc_map.name, static_cast<int>(mcmapping));
            SLEEP(300);
            return nullptr;
        }
#endif
        cached_fids.insert({mcmapping, sfId});
        return sfId;
	}
    
    jfieldID get_fid(jclass cls, mappingFields mcmapping, JNIEnv* env)
	{
#ifdef ENABLE_LOGGING
        if (!mappings::fields.contains(mcmapping))
        {
            LOGERROR("field mapping name was not found with index {}", static_cast<int>(mcmapping));
            SLEEP(300);
        }
#endif
        if (auto it = cached_fids.find(mcmapping); it != cached_fids.end())
        {
            return it->second;
        }

		auto it = mappings::fields.find(mcmapping);
		const mappings::MCMap& mc_map = it->second;

        auto fId = env->GetFieldID(cls, mc_map.name.c_str(), mc_map.sig.c_str());
#ifdef ENABLE_LOGGING
        if (!fId)
        {
            LOGERROR("fieldId is null with mapping name: {}, and index: {}", mc_map.name, static_cast<int>(mcmapping));
            SLEEP(300);
            return nullptr;
        }
#endif
        cached_fids.insert({ mcmapping, fId });

        return fId;
	}

	jfieldID get_fid(jobject obj, mappingFields name, JNIEnv* env)
	{
        auto objKlass = env->GetObjectClass(obj);
        if (!objKlass)
        {
#ifdef ENABLE_LOGGING
            LOGERROR("getting object class for fieldid returned null with mapping name: {}, and index: {}", mappings::fields[name].name, static_cast<int>(name));

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

        static auto xposid = env->GetMethodID(posclass, mappings::methods[mapping::Vec3X].name.c_str(), "()D");
        if (!xposid) return {-1, -1, -1};

        static auto yposid = env->GetMethodID(posclass, mappings::methods[mapping::Vec3Y].name.c_str(), "()D");
        static auto zposid = env->GetMethodID(posclass, mappings::methods[mapping::Vec3Z].name.c_str(), "()D");

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

        static auto xposid = env->GetMethodID(posclass, mappings::methods[mapping::Vec3IX].name.c_str(), "()I");
        if (!xposid) return {-1, 0, 0};

        static auto yposid = env->GetMethodID(posclass, mappings::methods[mapping::Vec3IY].name.c_str(), "()I");
        static auto zposid = env->GetMethodID(posclass, mappings::methods[mapping::Vec3IZ].name.c_str(), "()I");

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

        if (g_jvmti_env)
        {
            LOGDEBUG("======== fields ========");
            jint n = 0;
            jfieldID* ids = nullptr;
            auto err = g_jvmti_env->GetClassFields(klass, &n, &ids);
            if (err != JVMTI_ERROR_NONE)
                return; 

            for (jint i = 0; i < n; i++)
            {
                auto fid = ids[i];

                char* name = nullptr;
                char* sig = nullptr;
                char* g = nullptr; 
                if (g_jvmti_env->GetFieldName(klass, fid, &name, &sig, &g) != JVMTI_ERROR_NONE)
                    continue;

                LOGDEBUG("name: {}, sig: {}", name, sig);

				g_jvmti_env->Deallocate((unsigned char*)name);
				g_jvmti_env->Deallocate((unsigned char*)sig);
				g_jvmti_env->Deallocate((unsigned char*)g);

            }

            g_jvmti_env->Deallocate((unsigned char*)ids);
        }
    }
}
