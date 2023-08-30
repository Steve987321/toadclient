#include "pch.h"
#include "Toad/Toad.h"
#include "no_click_delay.h"

void toadll::CNoClickDelay::PreUpdate()
{
	Enabled = toad::no_click_delay::enabled;
	SLEEP(100);
}

void toadll::CNoClickDelay::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	if (toad::left_clicker::enabled)
	{
		SLEEP(100);
		return;
	}

	if (GetAsyncKeyState(VK_LBUTTON))
	{
		Invoke(MC);
	}

	SLEEP(10);
}

void toadll::CNoClickDelay::Invoke(const std::shared_ptr<Minecraft>& minecraft)
{
	minecraft->setLeftClickCounter(0);
}
