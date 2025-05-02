#include "pch.h"
#include "Toad/toad.h"
#include "no_click_delay.h"

void toad::CNoClickDelay::PreUpdate()
{
	WaitIsEnabled();
	WaitIsVerified();
}

void toad::CNoClickDelay::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	if (toad::settings.lc_enabled)
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

void toad::CNoClickDelay::Invoke(Minecraft* minecraft)
{
	minecraft->setLeftClickCounter(0);
}
