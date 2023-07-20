#include "pch.h"
#include "Toad/Toad.h"
#include "velocity.h"

using namespace toad;

namespace toadll
{
	void CVelocity::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
	{
		if (!velocity::enabled)
		{
			SLEEP(250);
			return;
		}

		// A flag to stop execution of the velocity or jump reset.
		// Is resetted after local player is ready to receive a hit again
		static bool StopFlag = false;

		if (velocity::jump_reset)
		{
			if (lPlayer->HurtTime > 0 && !StopFlag)
			{
				StopFlag = true;
				send_key(VK_SPACE);
				SLEEP(rand_int(40, 70));
				send_key(VK_SPACE, false);
			}
			else if (lPlayer->HurtTime == 0)
				StopFlag = false;

			SLEEP(1);
			return;
		}

		static int beginHurtTime = 0;

		if (int hurttime = lPlayer->HurtTime; hurttime > 0 && !StopFlag)
		{
			if (beginHurtTime < hurttime) beginHurtTime = hurttime;

			if (hurttime != beginHurtTime - (int)velocity::delay)
			{
				SLEEP(1);
				return;
			}
			if (rand_int(0, 100) > velocity::chance)
			{
				StopFlag = true;
				SLEEP(1);
				return;
			}

			//if (velocity::delay > 0) toad::preciseSleep(velocity::delay * 0.05f);

			auto motionX = lPlayer->Motion.x;
			auto newMotionX = motionX * (velocity::horizontal / 100); /* std::lerp(motionX, motionX * (velocity::horizontal / 100.f), 0.3f * partialTick);*/
			auto motionZ = lPlayer->Motion.z;
			auto newMotionZ = motionZ * (velocity::horizontal / 100); /*std::lerp(motionZ, motionZ * (velocity::horizontal / 100.f), 0.3f * partialTick);*/

			auto EditableLocalPlayer = MC->getLocalPlayer();
			if (!EditableLocalPlayer)
			{
				SLEEP(1);
				return;
			}

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

		SLEEP(1);
	}
}
