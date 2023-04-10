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

        [[nodiscard]] float dist(const vec3& v) const {
            return sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z));
        }


    };

	struct vec4 {
        vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

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

    // for logging
    inline std::ostream& operator<<(std::ostream& o, const vec2& v) {
        o << "(X:" << v.x << ", Y:" << v.y << ")";
        return o;
    }

    inline std::ostream& operator<<(std::ostream& o, const vec3& v) {
	    o << "(X:" << v.x << ", Y:" << v.y << ", Z:" << v.y << ")";
	    return o;
    }

	inline std::ostream& operator<<(std::ostream& o, const vec4& v) {
        o << "(X:" << v.x << ", Y:" << v.y << ", Z:" << v.y << ", W:" << v.w << ")";
        return o;
    }

    enum class minecraft_client
    {
        Lunar,
        Vanilla,
        Forge
    };

    enum class mappingFields
    {
        // .Minecraft
        theMcField,

        // .GameSettings
        fovField,

        // .Entity
        rotationYawField,
        rotationPitchField,
        prevRotationYawField,
        prevRotationPitchField,

        // jdouble
        motionXField,
        motionYField,
        motionZField,

        // .ActiveRenderInfo
        viewportField,
        projectionField,
		modelviewField
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

        // Timer
        partialTick
    };

    constexpr static auto PI = 3.14159265358979323846f;

}