#pragma once

#include <cmath>

namespace toadll
{
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

        // .Entity
        getPos,

        // .Vec3
        Vec3X,
        Vec3Y,
        Vec3Z
    };


}