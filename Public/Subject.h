#pragma once

enum class OBSERVER_NOTIFY_TYPE;

namespace Engine
{
	class ISubject abstract
	{
	public:
		virtual void Register_Observer(class IObserver* _pObserver) = 0;
		virtual void Remove_Observer(class IObserver* _pObserver) = 0;
		virtual void Notify_Observers(OBSERVER_NOTIFY_TYPE _eType, any _arg) = 0;
	};
};