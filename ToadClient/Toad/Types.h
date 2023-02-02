#pragma once

namespace toadll
{
    struct vec2 {
        vec2(float x, float y) : x(x), y(y) {}

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

    enum class minecraft_client
    {
        Lunar,
        Vanilla,
        Forge
    };

    enum class mapping
    {
        // .Minecraft
        getMinecraft,
        getWorld,
        getPlayer,
        getObjectMouseOver,

        // .World
        getPlayerEntities,
        getRenderManager,
        isAirBlock,

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

        // .rendermanager
        getRenderPosX,
        getRenderPosY,
        getRenderPosZ,

        // .Vec3
        Vec3X,
        Vec3Y,
        Vec3Z,

        //. Vec3I
    	Vec3IX,
        Vec3IY,
        Vec3IZ
    };

    constexpr static auto PI = 3.14159265358979323846f;

}