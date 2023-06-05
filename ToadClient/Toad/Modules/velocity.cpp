#include "pch.h"
#include "Toad/Toad.h"
#include "velocity.h"

namespace toadll
{
	void CVelocity::OnTick(const std::shared_ptr<c_Entity>& lPlayer)
	{
		if (!velocity::enabled) return;

		static bool StopFlag = false;
		static int beginHurtTime = 0;

		if (int hurttime = lPlayer->get_hurt_time(); hurttime > 0 && !StopFlag)
		{
			if (beginHurtTime < hurttime) beginHurtTime = hurttime;

			if (hurttime != beginHurtTime - (int)velocity::delay) return;
			if (toadll::rand_int(0, 100) > velocity::chance) { StopFlag = true; return; }

			//if (velocity::delay > 0) toad::preciseSleep(velocity::delay * 0.05f);

			auto motionX = lPlayer->get_motionX();
			auto newMotionX = motionX * (velocity::horizontal / 100); /* std::lerp(motionX, motionX * (velocity::horizontal / 100.f), 0.3f * partialTick);*/
			auto motionZ = lPlayer->get_motionZ();
			auto newMotionZ = motionZ * (velocity::horizontal / 100); /*std::lerp(motionZ, motionZ * (velocity::horizontal / 100.f), 0.3f * partialTick);*/

			if (abs(motionX) > 0)
				lPlayer->set_motionX(newMotionX);
			if (abs(motionZ) > 0)
				lPlayer->set_motionZ(newMotionZ);

			// TODO: separate horizontal and vertical velocity module in separate threads? 

			if (lPlayer->get_motionY() > 0) // normal velocity when going down 
				lPlayer->set_motionY(lPlayer->get_motionY() * (velocity::vertical / 100.f));

			StopFlag = true;

			/*if (velocity::vertical > 0 && velocity::horizontal > 0)
				timer -= partialTick;*/
		}
		else if (hurttime <= 0)
		{
			StopFlag = false;
			beginHurtTime = 0;
		}
	}
}
