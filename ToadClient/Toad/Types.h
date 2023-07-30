#pragma once

namespace toadll
{

struct Vec2 {
    Vec2(float x, float y) : x(x), y(y) {}
    Vec2() = default;

    float x, y;

    Vec2 operator+(const Vec2& v) const {
        return { x + v.x, y + v.y };
    }

    Vec2 operator-(const Vec2& v) const {
        return { x - v.x, y - v.y };
    }

    Vec2 operator*(float s) const {
        return { x * s, y * s};
    }

    Vec2 operator/(float s) const {
        return { x / s, y / s};
    }

    constexpr bool operator==(const Vec2& v) const{
        return x == v.x && y == v.y;
    }

    _NODISCARD float dist(const Vec2& v) const {
        return sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y));
    }
};

struct Vec3 {
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3() : x(0), y(0), z(0) {}

    float x, y, z;

    Vec3 operator+(const Vec3& v) const {
        return { x + v.x, y + v.y, z + v.z };
    }

    Vec3 operator-(const Vec3& v) const {
        return { x - v.x, y - v.y, z - v.z };
    }

    Vec3 operator*(float s) const {
        return { x * s, y * s, z * s };
    }

    Vec3 operator/(float s) const {
        return { x / s, y / s, z / s };
    }

	bool operator==(const Vec3& v) const {
        return fabs(x - v.x) < FLT_EPSILON && fabs(y - v.y) < FLT_EPSILON && fabs(z - v.z) < FLT_EPSILON;
    }

    _NODISCARD float dist(const Vec3& v) const {
        return sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z));
    }

    _NODISCARD float dot(const Vec3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    _NODISCARD Vec3 cross(const Vec3& v) const{
        return
        {
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
        };
    }

    static Vec3 normalize(const Vec3& v)
    {
        float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (length > 0.0f)
        {
            float invLength = 1.0f / length;
            return Vec3(v.x * invLength, v.y * invLength, v.z * invLength);
        }
        else
        {
            return Vec3(0.0f, 0.0f, 0.0f);
        }
    }
};

struct Vec4 {
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vec4() = default;
    float x, y, z, w;

    Vec4 operator+(const Vec4& v) const {
        return { x + v.x, y + v.y, z + v.z, w + v.w};
    }

    Vec4 operator-(const Vec4& v) const {
        return { x - v.x, y - v.y, z - v.z, w - v.w};
    }

    Vec4 operator*(float s) const {
        return { x * s, y * s, z * s, w * s };
    }

    Vec4 operator/(float s) const {
        return { x / s, y / s, z / s, w/ s };
    }
};

struct BBox
{
    BBox(const Vec3& min, const Vec3& max) : min(min), max(max) {}

    Vec3 min, max;
};

// for logging
inline std::ostream& operator<<(std::ostream& o, const Vec2& v) {
    o << "(X:" << v.x << ", Y:" << v.y << ")";
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const Vec3& v) {
    o << "(X:" << v.x << ", Y:" << v.y << ", Z:" << v.z << ")";
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const Vec4& v) {
    o << "(X:" << v.x << ", Y:" << v.y << ", Z:" << v.y << ", W:" << v.w << ")";
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const BBox& v){
    o << "(min: " << v.min << "), max(" << v.max << ")";
    return o;
}

struct Entity
{
    //std::string Name;

    bool Invis = false;

    Vec3 Pos = { 0,0,0 };
    Vec3 LastTickPos = { 0,0,0 };

    int HurtTime = 0;

    float Health = 0;

    float Pitch = 0;
    float Yaw = 0;

    jobject obj = nullptr;

};

struct LocalPlayer : Entity
{
    Vec3 Motion = {};
    std::string HeldItem;
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
    theWorldField,

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
    inventoryField,
    hurtTimeI,

    // jdouble
    motionXField,
    motionYField,
    motionZField,

    EntityPosX,
    EntityPosY,
    EntityPosZ,

    // .ActiveRenderInfo
    viewportField,
    projectionField,
	modelviewField,

    // Timer
    renderPartialTickField,

    blockPos
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
    getBlockAt,
    isAirBlock,
    rayTraceBlocks, // args: (jobject Vec3 from, jobject Vec3 direction, bool stopOnLiquid) returns: jobject MovingObjectPosition if block hit else null

    // .Block
    getBlockFromBlockState,
    getIdFromBlockStatic,

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
    getBlockPositionFromMovingBlock,

    setRotationYaw,
    setRotationPitch,
    setRotation,

    getBBox,

    getName,

    getMotionX,
    getMotionY,
    getMotionZ,

    isInvisible,

    // inv
    getStackInSlot,

    // .EntityRenderer
    disableLightmap,
    enableLightmap,

    // .EntityLivingBase
    getHeldItem,
    getHurtTime,
    getHealth,

    // .ActiveRenderInfo
    getRenderPos,

    // .Vec3
    Vec3Init, // args: double x, double y, double z 
    Vec3X,
    Vec3Y,
    Vec3Z,

    // .Vec3I
    Vec3IInit, // args: int x, int y, int z
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

constexpr static auto g_PI = 3.14159265358979323846f;

}