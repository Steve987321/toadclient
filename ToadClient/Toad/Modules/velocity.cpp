#include "pch.h"
#include "Toad/Toad.h"
#include "velocity.h"

using namespace toad;
using namespace toadll::math;

namespace toadll
{
	void CVelocity::PreUpdate()
	{
		CModule::PreUpdate();
	}

	void CVelocity::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
	{
		Enabled = velocity::enabled;
		if (!Enabled)
		{
			SLEEP(250);
			return;
		}

		// A flag to stop execution of the velocity or jump reset.
		// Is resetted after local player is ready to receive a hit again
		static bool StopFlag = false;

		if (velocity::jump_reset)
		{
			// jumping won't have any effect
			if (lPlayer->Motion.y < -0.1f)
				return;

			if (velocity::only_when_moving && std::fabs(lPlayer->Motion.x + lPlayer->Motion.z) < FLT_EPSILON)
				return;

			if (velocity::only_when_clicking && !GetAsyncKeyState(VK_LBUTTON))
				return;

			if (lPlayer->HurtTime > 0 && !StopFlag)
			{
				if (velocity::jump_press_chance < rand_int(0, 100))
				{
					StopFlag = true;
					return;
				}

				if (velocity::kite)
				{
					auto yaw = wrap_to_180(lPlayer->Yaw - 90);
					if (isDirectionAligned(yaw, lPlayer->Motion.x, lPlayer->Motion.z))
					{
						// stop
						StopFlag = true;
						return;
					}
				}

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

		// normal velocity

		// the hurttime value on player hit
		static int begin_hurt_time = 0;

		if (velocity::only_when_moving && std::fabs(lPlayer->Motion.x + lPlayer->Motion.z) < FLT_EPSILON)
			return;

		if (velocity::only_when_clicking && !GetAsyncKeyState(VK_LBUTTON))
			return;

		if (int hurttime = lPlayer->HurtTime; hurttime > 0 && !StopFlag)
		{
			if (begin_hurt_time < hurttime) begin_hurt_time = hurttime;

			if (hurttime != begin_hurt_time - (int)velocity::delay)
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
			// get updated

			auto EditableLocalPlayer = MC->getLocalPlayer();

			auto motionX = EditableLocalPlayer->getMotionX();
			auto motionZ = EditableLocalPlayer->getMotionZ();

			if (velocity::kite)
			{
				auto yaw = wrap_to_180(lPlayer->Yaw - 90);
				if (isDirectionAligned(yaw, motionX, motionZ))
				{
					// stop
					StopFlag = true;
					return;
				}
			}

			auto newMotionX = motionX * (velocity::horizontal / 100); /* std::lerp(motionX, motionX * (velocity::horizontal / 100.f), 0.3f * partialTick);*/
			auto newMotionZ = motionZ * (velocity::horizontal / 100); /*std::lerp(motionZ, motionZ * (velocity::horizontal / 100.f), 0.3f * partialTick);*/

			if (abs(motionX) > 0)
				EditableLocalPlayer->setMotionX(newMotionX);
			if (abs(motionZ) > 0)
				EditableLocalPlayer->setMotionZ(newMotionZ);

			constexpr auto vcheck = (100.f - 0.1f);
			if (velocity::vertical <= vcheck && lPlayer->Motion.y > 0) // normal velocity when going down 
			{
				EditableLocalPlayer->setMotionY(lPlayer->Motion.y * (velocity::vertical / 100.f));
			}
			StopFlag = true;
		}
		else if (hurttime <= 0)
		{
			StopFlag = false;
			begin_hurt_time = 0;
		}

		SLEEP(1);
	}

	bool CVelocity::isDirectionAligned(float yaw, float hdirx, float hdirz)
	{
		constexpr float threshold = -0.1f;

		float yawRad = yaw * g_PI / 180.f;

		float forwardX = std::cos(yawRad);
		float forwardZ = std::sin(yawRad);

		return forwardX * hdirx + forwardZ * hdirz < threshold;
	}

}
