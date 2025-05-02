#include "pch.h"
#include "helpers.h"
#include "Toad/toad.h"

namespace toad
{
	static std::random_device rd;

	float RandFloat(float min, float max)
	{
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(min, max);
		return dis(gen);
	}

	int RandInt(int min, int max)
	{
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(min, max);
		return dis(gen);
	}

	float Slerp(float start, float end, float t)
	{
		t = std::clamp(t, 0.0f, 1.0f);
		t = t * t * (3.0f - 2.0f * t);
		return start + (end - start) * t;
	}

	void PreciseSleep(double seconds)
	{
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

	void SendKey(WORD vk_key, bool send_down)
	{
		INPUT ip{ INPUT_KEYBOARD };

		ip.ki.wScan = 0;
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;
		ip.ki.wVk = vk_key;
		ip.ki.dwFlags = send_down ? 0 : KEYEVENTF_KEYUP;

		SendInput(1, &ip, sizeof(INPUT));
	}

	toad::Vec3 GetClosestPoint(const BBox& bb, const Vec3& from)
	{
		Vec3 closest;

		closest.x = std::clamp(from.x, bb.min.x, bb.max.x);
		closest.y = std::clamp(from.y, bb.min.y, bb.max.y);
		closest.z = std::clamp(from.z, bb.min.z, bb.max.z);

		return closest;
	}

	std::filesystem::path GetDocumentsFolder()
	{
		CHAR documents[MAX_PATH];
		HRESULT res = SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, documents);
		if (res == S_OK)
		{
			return documents;
		}
		return "";
	}

}