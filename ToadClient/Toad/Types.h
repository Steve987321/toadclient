#pragma once

namespace toadll
{
    struct vec2 {
        vec2(float x, float y) : x(x), y(y) {}
        vec2() = default;

        float x, y;

        vec2 operator+(const vec2& v) const {
            return { x + v.x, y + v.y };
        }

        vec2 operator-(const vec2& v) const {
            return { x - v.x, y - v.y };
        }

        vec2 operator*(float s) const {
            return { x * s, y * s};
        }

        vec2 operator/(float s) const {
            return { x / s, y / s};
        }

        constexpr bool operator==(const vec2& v) const{
            return x == v.x && y == v.y;
        }

        [[nodiscard]] float dist(const vec2& v) const {
            return sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y));
        }
    };

	struct vec3 {
        vec3(float x, float y, float z) : x(x), y(y), z(z) {}
        vec3() : x(0), y(0), z(0) {}

        float x, y, z;

        vec3 operator+(const vec3& v) const {
            return { x + v.x, y + v.y, z + v.z };
        }

        vec3 operator-(const vec3& v) const {
            return { x - v.x, y - v.y, z - v.z };
        }

        vec3 operator*(float s) const {
            return { x * s, y * s, z * s };
        }

        vec3 operator/(float s) const {
            return { x / s, y / s, z / s };
        }

        constexpr bool operator==(const vec3& v) const {
            return x == v.x && y == v.y && z == v.z;
        }

        _NODISCARD float dist(const vec3& v) const {
            return sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z));
        }

        _NODISCARD vec3 cross(const vec3& v) const{
            return
            {
                    y * v.z - z * v.y,
                    z * v.x - x * v.z,
                    x * v.y - y * v.x
            };
        }

        static vec3 normalize(const vec3& v)
        {
            float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
            if (length > 0.0f)
            {
                float invLength = 1.0f / length;
                return vec3(v.x * invLength, v.y * invLength, v.z * invLength);
            }
            else
            {
                return vec3(0.0f, 0.0f, 0.0f);
            }
        }
	};

	struct vec4 {
        vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        vec4() = default;
        float x, y, z, w;

        vec4 operator+(const vec4& v) const {
            return { x + v.x, y + v.y, z + v.z, w + v.w};
        }

        vec4 operator-(const vec4& v) const {
            return { x - v.x, y - v.y, z - v.z, w - v.w};
        }

        vec4 operator*(float s) const {
            return { x * s, y * s, z * s, w * s };
        }

        vec4 operator/(float s) const {
            return { x / s, y / s, z / s, w/ s };
        }
    };

    struct bbox
    {
        bbox(const vec3& min, const vec3& max) : min(min), max(max) {}

        vec3 min, max;
    };

    // for logging
    inline std::ostream& operator<<(std::ostream& o, const vec2& v) {
        o << "(X:" << v.x << ", Y:" << v.y << ")";
        return o;
    }

    inline std::ostream& operator<<(std::ostream& o, const vec3& v) {
	    o << "(X:" << v.x << ", Y:" << v.y << ", Z:" << v.z << ")";
	    return o;
    }

	inline std::ostream& operator<<(std::ostream& o, const vec4& v) {
        o << "(X:" << v.x << ", Y:" << v.y << ", Z:" << v.y << ", W:" << v.w << ")";
        return o;
    }

    inline std::ostream& operator<<(std::ostream& o, const bbox& v){
        o << "(min: " << v.min << "), max(" << v.max << ")";
        return o;
    }

    struct EntityT
    {
        std::string Name;
        std::string HeldItem;

        bool Invis = false;

        vec3 Pos = { 0,0,0 };
        vec3 LastTickPos = { 0,0,0 };

        int HurtTime = 0;

        float Pitch;
        float Yaw;

        jobject obj;

    };

    struct LocalPlayerT : EntityT
    {
        vec3 motion = {};
    };

    enum class minecraft_client
    {
        Lunar,
        Vanilla,
        Forge
    };

    struct MCMap
    {
        MCMap(const char* n, const char* s) : name(n), sig(s) {}

        const char* name;
        const char* sig;
    };

    enum class mappingFields
    {
        // .Minecraft
        theMcField,
        currentScreenField,
        thePlayerField,

        // .World
        playerEntitiesField,

        // .GameSettings
        fovField,

        // .Entity
        rotationYawField,
        rotationPitchField,
        prevRotationYawField,
        prevRotationPitchField,
        lastTickPosXField,
        lastTickPosYField,
        lastTickPosZField,

        // jdouble
        motionXField,
        motionYField,
        motionZField,

        // .ActiveRenderInfo
        viewportField,
        projectionField,
		modelviewField,

        // Timer
        renderPartialTickField,
    };

    enum class static_mapping
    {
	    
    };

    enum class mapping
    {
        // .Minecraft
        getMinecraft,
        getWorld,
        getPlayer,
        getGameSettings,
        getObjectMouseOver,
        getEntityRenderer,
        getTimer,

        // objectmouseover
        getEntityHit,

        // General
        toString,

        // .World
        getPlayerEntities,
        getRenderManager,
        isAirBlock,

        // player
        getOpenContainer,

        // .GameSettings
        setGamma,

        // .Entity
        getPos,
        getRotationYaw,
        getRotationPitch,

        getBlockPosition,
        getBlockPos,

        setRotationYaw,
        setRotationPitch,
        setRotation,

        getBBox,

        getInventory,

        getName,

        getMotionX,
        getMotionY,
        getMotionZ,

        isInvisible,

        // .EntityRenderer
        disableLightmap,
        enableLightmap,

        // .EntityLivingBase
        getHeldItem,
        getHurtTime,

        // .ActiveRenderInfo
        getRenderPos,

        // .Vec3
        Vec3X,
        Vec3Y,
        Vec3Z,

        //. Vec3I
    	Vec3IX,
        Vec3IY,
        Vec3IZ,

        // bounding box
        bboxMinX,
        bboxMinY,
        bboxMinZ,
        bboxMaxX,
        bboxMaxY,
        bboxMaxZ,

        // Timer
        partialTick,
    };

    constexpr static auto PI = 3.14159265358979323846f;

}