#include "IEventManager.h"
#include <map>
#include <vector>
#include <queue>

#ifndef GAME_EVENT_MANAGER_H
#define GAME_EVENT_MANAGER_H

class GameEventManager :
	public IEventManager
{
public:
	GameEventManager(void);
	~GameEventManager(void);

	void AddListener(IEventListener * pListener, const EventType& eventType);
	void RemoveListener(IEventListener * pListener, const EventType& eventType);

	void Trigger(IEvent * pEvent);
	void QueueEvent(IEvent * pEvent);

	void Tick(void);
private:
	std::map<int, std::vector<IEventListener*>> lsnrMap;

	std::queue<IEvent*> event_queue;
};

#endif