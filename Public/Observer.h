#pragma once

enum class OBSERVER_NOTIFY_TYPE;

namespace Engine
{
	class IObserver abstract
	{
	public:
		virtual void Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg) = 0;
	};
};