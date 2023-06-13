#include "pch.h"
#include "Toad/Toad.h"
#include "velocity.h"

using namespace toad;

namespace toadll
{
	void CVelocity::OnTick(const std::shared_ptr<LocalPlayerT>& lPlayer)
	{
		if (!velocity::enabled)
		{
			SLOW_SLEEP(250);
			return;
		}

		static bool StopFlag = false;

		if (velocity::jump_reset)
		{
			if (lPlayer->HurtTime > 0 && !StopFlag)
			{
				StopFlag = true;
				SendKey(VK_SPACE);
				SLOW_SLEEP(rand_int(40, 70));
				SendKey(VK_SPACE, false);
			}
			else if (lPlayer->HurtTime == 0)
				StopFlag = false;
			return;
		}

		static int beginHurtTime = 0;

		if (int hurttime = lPlayer->HurtTime; hurttime > 0 && !StopFlag)
		{
			if (beginHurtTime < hurttime) beginHurtTime = hurttime;

			if (hurttime != beginHurtTime - (int)velocity::delay) return;
			if (rand_int(0, 100) > velocity::chance) { StopFlag = true; return; }

			//if (velocity::delay > 0) toad::preciseSleep(velocity::delay * 0.05f);

			auto motionX = lPlayer->Motion.x;
			auto newMotionX = motionX * (velocity::horizontal / 100); /* std::lerp(motionX, motionX * (velocity::horizontal / 100.f), 0.3f * partialTick);*/
			auto motionZ = lPlayer->Motion.z;
			auto newMotionZ = motionZ * (velocity::horizontal / 100); /*std::lerp(motionZ, motionZ * (velocity::horizontal / 100.f), 0.3f * partialTick);*/

			auto EditableLocalPlayer = Minecraft->get_localplayer();
			if (!EditableLocalPlayer)
				return;

			if (abs(motionX) > 0)
				EditableLocalPlayer->setMotionX(newMotionX);
			if (abs(motionZ) > 0)
				EditableLocalPlayer->setMotionZ(newMotionZ);

			// TODO: separate horizontal and vertical velocity module in separate threads? 
			if (lPlayer->Motion.y > 0) // normal velocity when going down 
				EditableLocalPlayer->setMotionY(lPlayer->Motion.y * (velocity::vertical / 100.f));

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
