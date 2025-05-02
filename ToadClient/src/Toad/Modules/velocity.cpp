#include "pch.h"
#include "Toad/toad.h"
#include "velocity.h"

using namespace toad;
using namespace toad::math;

namespace toad
{
	void CVelocity::PreUpdate()
	{
		WaitIsEnabled();
		WaitIsVerified();
		CModule::PreUpdate();
	}

	void CVelocity::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
	{
		// A flag to stop execution of the velocity or jump reset.
		// Is resetted after local player is ready to receive a hit again
		static bool StopFlag = false;

		if (settings.vel_jump_reset)
		{
			// jumping won't have any effect
			if (lPlayer->Motion.y < -0.1f)
				return;

			if (settings.vel_only_when_moving && std::fabs(lPlayer->Motion.x + lPlayer->Motion.z) < FLT_EPSILON)
				return;

			if (settings.vel_only_when_clicking && !GetAsyncKeyState(VK_LBUTTON))
				return;

			if (lPlayer->HurtTime > 0 && !StopFlag)
			{
				if (settings.vel_jump_press_chance < RandInt(0, 100))
				{
					StopFlag = true;
					return;
				}

				if (settings.vel_kite)
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
				SendKey(VK_SPACE);
				SLEEP(RandInt(40, 70));
				SendKey(VK_SPACE, false);
			}
			else if (lPlayer->HurtTime == 0)
				StopFlag = false;

			SLEEP(1);
			return;
		}

		// normal velocity

		// the hurttime value on player hit
		static int begin_hurt_time = 0;

		if (settings.vel_only_when_moving && std::fabs(lPlayer->Motion.x + lPlayer->Motion.z) < FLT_EPSILON)
			return;

		if (settings.vel_only_when_clicking && !GetAsyncKeyState(VK_LBUTTON))
			return;

		if (int hurttime = lPlayer->HurtTime; hurttime > 0 && !StopFlag)
		{
			if (begin_hurt_time < hurttime) begin_hurt_time = hurttime;

			if (hurttime != begin_hurt_time - settings.vel_delay)
			{
				SLEEP(1);
				return;
			}
			if (RandInt(0, 100) > settings.vel_chance)
			{
				StopFlag = true;
				SLEEP(1);
				return;
			}

			//if (settings.vel_delay > 0) toad::preciseSleep(settings.vel_delay * 0.05f);
			// get updated

			auto EditableLocalPlayer = MC->getLocalPlayer();

			auto motionX = EditableLocalPlayer->getMotionX();
			auto motionZ = EditableLocalPlayer->getMotionZ();

			if (settings.vel_kite)
			{
				auto yaw = wrap_to_180(lPlayer->Yaw - 90);
				if (isDirectionAligned(yaw, motionX, motionZ))
				{
					// stop
					StopFlag = true;
					return;
				}
			}

			auto newMotionX = motionX * (settings.vel_horizontal / 100); /* std::lerp(motionX, motionX * (settings.vel_horizontal / 100.f), 0.3f * partialTick);*/
			auto newMotionZ = motionZ * (settings.vel_horizontal / 100); /*std::lerp(motionZ, motionZ * (settings.vel_horizontal / 100.f), 0.3f * partialTick);*/

			if (abs(motionX) > 0)
				EditableLocalPlayer->setMotionX(newMotionX);
			if (abs(motionZ) > 0)
				EditableLocalPlayer->setMotionZ(newMotionZ);

			constexpr auto vcheck = (100.f - 0.1f);
			if (settings.vel_vertical <= vcheck && lPlayer->Motion.y > 0) // normal velocity when going down 
			{
				EditableLocalPlayer->setMotionY(lPlayer->Motion.y * (settings.vel_vertical / 100.f));
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
