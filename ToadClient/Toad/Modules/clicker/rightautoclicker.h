#pragma once

namespace toadll
{

class CRightAutoClicker SET_MODULE_CLASS(CRightAutoClicker), public CClickerBase
{
protected:
	inline static Randomization rand = Randomization(
		0,
		0,
		20,
		50,
		0,
		false,
		0,
	{
			Inconsistency(10.f, 30.f , 70, 35),
			Inconsistency(20.f, 40.f , 60, 50),

		},
	{
			Inconsistency(30.f, 50.f , 70, 50),
			Inconsistency(60.f, 80.f , 50, 100),

			Inconsistency(-10.f, 0, 60, 50),
		},
	{
			Boost(0.5f, 120 , 5, {100, 150}, 1),
			Boost(0.4f, 160, 8, {150, 200}, 2),
		}
	);

protected:
	inline Randomization& get_rand() override
	{
		return rand;
	}

	bool mouse_down() override;
	void mouse_up() override;

public:
	void Update(const std::shared_ptr<LocalPlayerT>& lPlayer) override;

	static void SetDelays(int cps);

};

}


