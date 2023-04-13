#include "pch.h"
#include "Toad/Toad.h"
#include "velocity.h"

namespace toadll
{
	void CVelocity::Update(const std::shared_ptr<c_Entity>& lPlayer, float partialTick)
	{
		if (!velocity::enabled) return;

		static bool once = false;

		if (int hurttime = lPlayer->get_hurt_time(); hurttime > 0 && !once)
		{
			if (timer <= 15)
			{
				timer += partialTick;
				std::cout << timer << std::endl;
				return;
			}

			if (toad::rand_int(0, 100) > velocity::chance) { once = true; return; }

			if (velocity::delay > 0) toad::preciseSleep(velocity::delay * 0.05f);

			auto motionX = lPlayer->get_motionX();
			auto newMotionX = std::lerp(motionX, motionX * (velocity::horizontal / 100.f), 0.3f);
			auto motionZ = lPlayer->get_motionZ();
			auto newMotionZ = std::lerp(motionZ, motionZ * (velocity::horizontal / 100.f), 0.3f);

			if (abs(motionX) > 0)
				lPlayer->set_motionX(newMotionX);
			if (abs(motionZ) > 0)
				lPlayer->set_motionZ(newMotionZ);

			// TODO: separate horizontal and vertical velocity module in separate threads? 

			if (lPlayer->get_motionY() > 0) // normal velocity when going down 
				lPlayer->set_motionY(lPlayer->get_motionY() * (velocity::vertical / 100.f));

			timer = 0;
			/*if (velocity::vertical > 0 && velocity::horizontal > 0)
				timer -= partialTick;*/
		}
		else if (hurttime <= 0)
		{
			once = false;
		}
	}
}
