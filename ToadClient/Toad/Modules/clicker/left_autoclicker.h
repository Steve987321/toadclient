#pragma once

namespace toadll
{

class CLeftAutoClicker SET_MODULE_CLASS(CLeftAutoClicker), public CClickerBase
{
public:
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

public:
	// returns a reference to current randomization values
	static Randomization& GetRand();

	// update clicker rand delays by cps 
	static void SetDelays(int cps);

private:
	inline static Randomization m_rand = Randomization(
		0,
		0,
		20,
		50,
		0,
		false,
		0,
	{
			Inconsistency( 10.f, 40.f , 70, 35),
			Inconsistency( 20.f, 40.f , 60, 50),
			Inconsistency( 40.f, 60.f, 50, 150),

			Inconsistency(-10.f, 0    , 50, 40),
		},
	{
			Inconsistency(30.f, 50.f , 70, 50),
			Inconsistency(60.f, 80.f , 50, 100),

			Inconsistency( -10.f, 0, 60, 50),
			Inconsistency( -15.f, 0, 40, 60)
		},
	{
			Boost(1.2f, 50 , 3, {100, 150}, 0),
			Boost(0.5f, 80 , 5, {100, 150}, 1),
			Boost(0.4f, 100, 5, {150, 200}, 2),
			Boost(1.0f, 120, 5, {150, 200}, 3),

			// DROPS
			Boost(-1.0f, 190, 5, {150, 200}, 4),
			Boost(-1.5f, 50, 3, {100, 200}, 5),

		}		
	);

	inline Randomization& get_rand() override
	{
		return m_rand;
	}

private:
	// mouse press functions with randomization 
	inline bool mouse_down() override;
	inline void mouse_up() override;

	// No randomization on these, because they are used for block hitting and not for clicking
	static inline void right_mouse_down();
	static inline void right_mouse_up();
};

}
