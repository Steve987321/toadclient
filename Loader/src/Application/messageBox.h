#pragma once

#include "utils/utils.h"

namespace toad
{
	class c_MessageBox
	{
	private:
		const char* m_Title = "";
		const char* m_Msg = "";

		bool& m_Condition;

		bool m_once = false;

		utils::mboxType m_Type = utils::mboxType::NONE;

	private:
		void show();

	public:
		explicit c_MessageBox(const char* title, const char* msg, bool& condition, utils::mboxType msgType = utils::mboxType::NONE)
		: m_Title(title), m_Msg(msg), m_Condition(condition), m_Type(msgType)
		{
			show();
		}
	};
}

