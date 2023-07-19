#pragma once

namespace toadll
{
    inline float RandFloat(float min, float max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    inline int RandInt(int min, int max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(min, max);
        return dis(gen);
    }

    /// Smooth interpolation  
    inline float slerp(float start, float end, float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        t = t * t * (3.0f - 2.0f * t);
        return start + (end - start) * t;
    }

    /// very precise
    inline void preciseSleep(double seconds) {
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
    inline void SendKey(WORD vk_key, bool send_down = true)
    {
        static INPUT ip{INPUT_KEYBOARD};

        ip.ki.wScan = 0; 
        ip.ki.time = 0;
        ip.ki.dwExtraInfo = 0;
        ip.ki.wVk = vk_key;
        ip.ki.dwFlags = send_down ? 0 : KEYEVENTF_KEYUP;

        SendInput(1, &ip, sizeof(INPUT));
    }
}