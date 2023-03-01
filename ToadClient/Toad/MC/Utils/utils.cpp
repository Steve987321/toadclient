#include "pch.h"
#include "utils.h"

#include "Toad/Toad.h"

namespace toadll
{
	void draw::drawRect(float x1, float y1, float x2, float y2)
	{
        glBegin(GL_LINE_LOOP);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x1, y2);
        glVertex2f(x2, y2 );
        glEnd();
	}

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

    jmethodID get_mid(const jclass& cls, mapping name)
    {
        return env->GetMethodID(cls, mappings::findName(name), mappings::findSig(name));
    }

    jmethodID get_mid(const jobject& obj, mapping name)
    {
        auto objKlass = env->GetObjectClass(obj);
        auto mid = get_mid(objKlass, name);
        env->DeleteLocalRef(objKlass);
        return mid;
    }

    jmethodID get_static_mid(const jclass& cls, mapping name)
    {
        return env->GetStaticMethodID(cls, mappings::findName(name), mappings::findSig(name));
    }

    jfieldID get_static_fid(const jclass& cls, mappingFields name)
	{
        return env->GetStaticFieldID(cls, mappings::findNameField(name), mappings::findSigField(name));
	}
    
    jfieldID get_fid(const jclass& cls, mappingFields name)
	{
        return env->GetFieldID(cls, mappings::findNameField(name), mappings::findSigField(name));
	}

	jfieldID get_fid(const jobject& obj, mappingFields name)
	{
        return get_fid(env->GetObjectClass(obj), name);
	}
    

    vec3 to_vec3(const jobject& vecObj)
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

	vec3 to_vec3i(const jobject& vecObj)
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

    bool WorldToScreen(const vec3& source, const vec3& target, const vec2& viewAngles, int fov, vec2& screenpos)
    {
        auto const get_Hfov = [](float width, float height, float fov) -> float
        {
            auto r = width / height;
            return fov * (2 * atan(tan(fov / 2) / r));
        };
        auto const get_angle = [=](vec3 from, vec3 target) -> vec2
        {
            /*vec2 angles{0,0};
            vec3 delta = source - target;
            float hyp = source.dist(target);
            angles.x = (float)(std::atan(delta.z / hyp) * 180.0f / PI);
            angles.y = (float)(std::atan(delta.y / delta.x) * 180.0f / PI);

            if (delta.x >= 0.0f)
                angles.y += 180.0f;*/

            //return angles;

            float d_x = target.x - from.x;
            float d_y = target.y - from.y;
            float d_z = target.z - from.z;

            float hypothenuse = sqrt(d_x * d_x + d_z * d_z);
            float yaw = atan2(d_z, d_x) * 180.f / PI - 90.f;
            float pitch = -atan2(d_y, hypothenuse) * 180 / PI;

            return { yaw, pitch };
        };

        int hGameRes = 856;
        int vGameRes = 512;

        float hFov = get_Hfov(hGameRes, vGameRes, fov);
        //p_Log->LogToConsole(std::to_string(hFov).c_str());
        float vFov = fov;

        auto [yaw, pitch] = get_angles(source, target);

        float yawDiff = wrap_to_180(-(viewAngles.x - yaw));
        float pitchDiff = wrap_to_180(-(viewAngles.y - pitch));

        vec2 deltaAngles = {yawDiff, pitchDiff};
    /*    std::stringstream ss;
        ss << deltaAngles;
        p_Log->LogToConsole(ss.str().c_str());*/

        float hOffset = std::tan(deltaAngles.x / vFov / 2) * std::cos(hFov / 2) / std::sin(hFov / 2) * (hGameRes / 2);
        p_Log->LogToConsole(std::to_string(hOffset));
        float hScreenPos = hGameRes / 2 + hOffset;

        float vOffset = std::tan(deltaAngles.y / hFov / 2) * std::cos(vFov / 2) / std::sin(vFov / 2) * (vGameRes / 2);
        float vScreenPos = vGameRes / 2 + vOffset;

        screenpos.x = hScreenPos;
        screenpos.y = vScreenPos;

        return true;
    }

    void loop_through_class(const jclass klass)
    {
#ifndef _DEBUG
        return;
#endif

        for (auto i = 0; i < jvmfunc::oJVM_GetClassMethodsCount(env, klass); i++)
        {
            std::cout << "name: " << jvmfunc::oJVM_GetMethodIxNameUTF(env, klass, i) << " sig: " << jvmfunc::oJVM_GetMethodIxSignatureUTF(env, klass, i) << " args size: " << jvmfunc::oJVM_GetMethodIxArgsSize(env, klass, i) << std::endl;
		}

    }
}
