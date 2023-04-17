#include "pch.h"
#include "mcutils.h"

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

        env->DeleteLocalRef(array_elements);
        env->DeleteLocalRef(thread_clazz);
        env->DeleteLocalRef(thread);
        env->DeleteLocalRef(threadgroup_clazz);
        env->DeleteLocalRef(arrayD);
        env->DeleteLocalRef(threadgroup_obj);

        return env->FindClass(clsName);
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

    jmethodID get_mid(jclass cls, mapping name)
    {
        return env->GetMethodID(cls, mappings::findName(name), mappings::findSig(name));
    }

    jmethodID get_mid(jobject obj, mapping name)
    {
        auto objKlass = env->GetObjectClass(obj);
        auto mid = get_mid(objKlass, name);
        env->DeleteLocalRef(objKlass);
        return mid;
    }

    jmethodID get_static_mid(jclass cls, mapping name)
    {
        return env->GetStaticMethodID(cls, mappings::findName(name), mappings::findSig(name));
    }

    jfieldID get_static_fid(jclass cls, mappingFields name)
	{
        return env->GetStaticFieldID(cls, mappings::findNameField(name), mappings::findSigField(name));
	}
    
    jfieldID get_fid(jclass cls, mappingFields name)
	{
        return env->GetFieldID(cls, mappings::findNameField(name), mappings::findSigField(name));
	}

	jfieldID get_fid(jobject obj, mappingFields name)
	{
        return get_fid(env->GetObjectClass(obj), name);
	}
    

    vec3 to_vec3(jobject vecObj)
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

	vec3 to_vec3i(jobject vecObj)
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

    double calculateHorizontalFOV(double screenHeight, double screenWidth, double verticalFOV) {
        double aspectRatio = screenWidth / screenHeight;
        double verticalFOVRad = verticalFOV * PI / 180; // convert to radians
        double horizontalFOVRad = 2 * atan(tan(verticalFOVRad / 2) * aspectRatio);
        double horizontalFOV = horizontalFOVRad * 180 / PI; // convert back to degrees
        return horizontalFOV;
    }

    //bool WorldToScreen(const vec3& worldpos, vec2& screen, GLfloat modelView[15], GLfloat projection[15], GLint viewPort[3])
    //{
    //    const auto Multiply = [](const vec4& vec, const GLfloat mat[15]) -> vec4
    //    {
    //        return {
	//            vec.x * mat[0] + vec.y * mat[4] + vec.z * mat[8] + vec.w * mat[12],
    //            vec.x * mat[1] + vec.y * mat[5] + vec.z * mat[9] + vec.w * mat[13],
    //            vec.x * mat[2] + vec.y * mat[6] + vec.z * mat[10] + vec.w * mat[14],
    //            vec.x * mat[3] + vec.y * mat[7] + vec.z * mat[11] + vec.w * mat[15]
    //        };
    //    };
    //
    //    auto clipSpacePos = Multiply(Multiply(vec4(worldpos.x, worldpos.y, worldpos.z, 1.0f), modelView), projection);
    //    auto ndcSpacePos = vec3(clipSpacePos.x / clipSpacePos.w, clipSpacePos.y / clipSpacePos.w, clipSpacePos.z / clipSpacePos.w);
    //
    //    if (ndcSpacePos.z < -1.0 || ndcSpacePos.z > 1.0)
    //    {
    //        return false;
    //    }
    //
    //    screen.x = (ndcSpacePos.x + 1.0f) / 2.0f * viewPort[2];
    //    screen.y = (1.0f - ndcSpacePos.y) / 2.0f * viewPort[3];
    //    return true;
    //}

    bool WorldToScreen(const vec3& source, const vec3& target, const vec2& viewAngles, float fov, vec2& screenpos)
    {
        // Get screen dimensions
        int hGameRes = screen_width;
        int vGameRes = screen_height;

        // Calculate aspect ratio
        float aspectRatio = hGameRes / vGameRes;

        // Calculate horizontal and vertical field of view in radians
        auto hFov = calculateHorizontalFOV(screen_height, screen_width, fov);
        float hFovRad = hFov * PI / 180.f;
        float vFovRad = 2 * atan(tan(fov * PI / 180.f / 2) / aspectRatio);

        // Get view angles
        float yaw = viewAngles.x * PI / 180.f + 1.55f;
        float pitch = viewAngles.y * PI / 180.f;

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

    void loop_through_class(const jclass klass)
    {
        for (auto i = 0; i < jvmfunc::oJVM_GetClassMethodsCount(env, klass); i++)
        {
            std::cout << "name: " << jvmfunc::oJVM_GetMethodIxNameUTF(env, klass, i) << " sig: " << jvmfunc::oJVM_GetMethodIxSignatureUTF(env, klass, i) << " args size: " << jvmfunc::oJVM_GetMethodIxArgsSize(env, klass, i) << std::endl;
		}

    }
}
