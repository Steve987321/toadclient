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
        if (!xposid) return {-1, 0, 0};

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

    double calculateHorizontalFOV(double screenHeight, double screenWidth, double verticalFOV) {
        double aspectRatio = screenWidth / screenHeight;
        double verticalFOVRad = verticalFOV * g_PI / 180; // convert to radians
        double horizontalFOVRad = 2 * atan(tan(verticalFOVRad / 2) * aspectRatio);
        double horizontalFOV = horizontalFOVRad * 180 / g_PI; // convert back to degrees
        return horizontalFOV;
    }

    bool WorldToScreen(const Vec3& source, const Vec3& target, const Vec2& viewAngles, float fov, Vec2& screenpos)
    {
        // Get screen dimensions
        int hGameRes = g_screen_width;
        int vGameRes = g_screen_height;

        // Calculate aspect ratio
        float aspectRatio = hGameRes / vGameRes;

        // Calculate horizontal and vertical field of view in radians
        auto hFov = calculateHorizontalFOV(g_screen_height, g_screen_width, fov);
        float hFovRad = hFov * g_PI / 180.f;
        float vFovRad = 2 * atan(tan(fov * g_PI / 180.f / 2) / aspectRatio);

        // Get view angles
        float yaw = viewAngles.x * g_PI / 180.f + 1.55f;
        float pitch = viewAngles.y * g_PI / 180.f;

        // Get the position of the target relative to the source
        float dx = source.x - target.x;
        float dy = source.y - target.y;
        float dz = source.z - target.z;

        // Calculate the distance between the source and the target
        float distance = source.dist(target);

        // Calculate yaw and pitch angles between the source and the target
        float pitchRad = asin(dy / distance);
        float yawRad = atan2(dz, dx);

        // Calculate the delta angles between the view angles and the target angles
        float deltaYawRad = yawRad - yaw;
        float deltaPitchRad = pitchRad - pitch;

        // Calculate the screen coordinates

        float x = tan(deltaYawRad) / tan(hFovRad / 2.f) * (hGameRes / 2.f) * aspectRatio;
        float y =  -tan(deltaPitchRad) / tan(vFovRad / 2.f) * (vGameRes / 2.f) * aspectRatio;
        // Clamp the coordinates to the screen bounds
        /*if (x < -hGameRes / 2.f) x = -hGameRes / 2.f;
        if (x > hGameRes / 2.f) x = hGameRes / 2.f;
        if (y < -vGameRes / 2.f) y = -vGameRes / 2.f;
        if (y > vGameRes / 2.f) y = vGameRes / 2.f;*/
        // Convert to screen coordinates
        screenpos.x = hGameRes / 2.f + x;
        screenpos.y = vGameRes / 2.f - y;

        return true;
    }

    Vec4 Multiply(const Vec4& vec, const std::vector<float>& mat)
    {
        return {
	        vec.x * mat[0] + vec.y * mat[4] + vec.z * mat[8] + vec.w * mat[12],
            vec.x * mat[1] + vec.y * mat[5] + vec.z * mat[9] + vec.w * mat[13],
            vec.x * mat[2] + vec.y * mat[6] + vec.z * mat[10] + vec.w * mat[14],
            vec.x * mat[3] + vec.y * mat[7] + vec.z * mat[11] + vec.w * mat[15]
        };
    }

    bool WorldToScreen(const Vec3& worldPos, Vec2& screen, const std::vector<float>& modelView, const std::vector<float>& projection, int width, int height)
    {
        // csp = Clip Space Position
        Vec4 csp = Multiply(
            Multiply(
                Vec4{ worldPos.x, worldPos.y, worldPos.z, 1.0f },
                modelView
            ),
            projection
        );

        // ndc = Native Device Coordinate
        Vec3 ndc{
            csp.x / csp.w,
            csp.y / csp.w,
            csp.z / csp.w
        };

        //Logger::Log("NDC.Z: " + std::to_string(ndc.z));

        if (ndc.z > 1 && ndc.z < 1.15) {
            screen = Vec2{
                ((ndc.x + 1.0f) / 2.0f) * width,
                ((1.0f - ndc.y) / 2.0f) * height,
            };
            return true;
        }

        return false;
    }

    void loop_through_class(const jclass klass, JNIEnv* env)
    {
        for (auto i = 0; i < jvmfunc::oJVM_GetClassMethodsCount(env, klass); i++)
        {
            LOGDEBUG("name: {}, sig: {} args size: %d", jvmfunc::oJVM_GetMethodIxNameUTF(env, klass, i), jvmfunc::oJVM_GetMethodIxSignatureUTF(env, klass, i), jvmfunc::oJVM_GetMethodIxArgsSize(env, klass, i));
		}

    }
}
