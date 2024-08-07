#pragma once

#include "types.h"

namespace toadll
{
    // helper functions 


inline std::random_device rd;
inline std::mt19937 gen(rd());

inline float rand_float(float min, float max)
{
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

inline int rand_int(int min, int max)
{
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}

// Smooth interpolation  
inline float slerp(float start, float end, float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    t = t * t * (3.0f - 2.0f * t);
    return start + (end - start) * t;
}

// very precise
inline void precise_sleep(double seconds) {
    using namespace std;
    using namespace std::chrono;

    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;

    while (seconds > estimate) {
        auto start = high_resolution_clock::now();
        this_thread::sleep_for(milliseconds(1));
        auto end = high_resolution_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double delta = observed - mean;
        mean += delta / count;
        m2 += delta * (observed - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    auto start = high_resolution_clock::now();
    while ((high_resolution_clock::now() - start).count() / 1e9 < seconds);
}

/// Sends a keyboard key press
///
/// @param vk_key keycode to send
/// @param send_down whether we want to send the key down or up
inline void send_key(WORD vk_key, bool send_down = true)
{
    static INPUT ip{INPUT_KEYBOARD};

    ip.ki.wScan = 0; 
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.wVk = vk_key;
    ip.ki.dwFlags = send_down ? 0 : KEYEVENTF_KEYUP;

    SendInput(1, &ip, sizeof(INPUT));
}

inline Vec3 get_closest_point(const BBox& bb, const Vec3& from)
{
	Vec3 closestPoint;

	// calculate the closest point on each axis
	closestPoint.x = std::clamp(from.x, bb.min.x, bb.max.x);
	closestPoint.y = std::clamp(from.y, bb.min.y, bb.max.y);
	closestPoint.z = std::clamp(from.z, bb.min.z, bb.max.z);

	return closestPoint;
}

}