#pragma once

namespace toadll
{

class CRightAutoClicker SET_MODULE_CLASS(CRightAutoClicker), public CClickerBase
{
public:
	CRightAutoClicker();

public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

public:
	// returns a reference to current randomization values
	static Randomization& GetRand();

	// update clicker rand delays by cps 
	static void SetDelays(int cps);

private:
	// TODO: is the same as left clicker 
	inline static Randomization m_rand = Randomization(
		0,
		0,
		20,
		50,
		0,
		0,
	{
			Inconsistency(10.f, 40.f , 70, 35),
			Inconsistency(20.f, 40.f , 60, 50),
			Inconsistency(40.f, 60.f , 50, 150),

			Inconsistency(-10.f, 0   , 50, 40),
		},
	{
			Inconsistency(30.f, 50.f , 70, 50),
			Inconsistency(60.f, 80.f , 50, 100),

			Inconsistency(-10.f, 0, 60, 50),
			Inconsistency(-15.f, 0, 40, 60)
		},
	{
			Boost(1.5f, 50 , 3, 100, 150, 0),
			Boost(0.5f, 80 , 5, 100, 150, 1),
			Boost(0.4f, 100, 5, 150, 200, 2),
			Boost(1.0f, 120, 5, 150, 200, 3),

			// DROPS
			Boost(-1.0f, 190, 5, 150, 200, 4),
			Boost(-1.5f, 50 , 3, 100, 200, 5),
			Boost(-1.5f, 150, 5, 100, 200, 5),

		}
	);

private:
	inline Randomization& get_rand() override
	{
		return m_rand;
	}

	bool mouse_down() override;
	void mouse_up() override;
};

}


