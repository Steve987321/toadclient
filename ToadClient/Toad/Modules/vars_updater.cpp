#include "pch.h"
#include "Toad/Toad.h"
#include "vars_updater.h"

using namespace toad;

void toadll::CVarsUpdater::Update(const std::shared_ptr<c_Entity>& lPlayer)
{
	CURSORINFO ci{ sizeof(CURSORINFO) };
	if (GetCursorInfo(&ci))
	{
		auto handle = reinterpret_cast<int>(ci.hCursor);
		g_is_cursor_shown = (handle > 50000 && handle < 1000000 || handle == 13961697);
		SLOW_SLEEP(1);
	}
}
