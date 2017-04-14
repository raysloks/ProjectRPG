#include "IEvent.h"
#include "IEventListener.h"
#include "GameEventManager.h"
#include <algorithm>

GameEventManager::GameEventManager(void)
{
}

GameEventManager::~GameEventManager(void)
{
}

void GameEventManager::AddListener(IEventListener * pListener, const EventType& eventType)
{
	lsnrMap[eventType.iType].push_back(pListener);
}

void GameEventManager::RemoveListener(IEventListener * pListener, const EventType& eventType)
{
	std::vector<IEventListener*>& vec = lsnrMap[eventType.iType];
	vec.erase(std::remove(vec.begin(), vec.end(), pListener), vec.end());
}

void GameEventManager::Trigger(IEvent * pEvent)
{
	std::vector<IEventListener*>& vec = lsnrMap[pEvent->GetType().iType];
	for (int i=0;i<vec.size();++i) {
		vec.at(i)->HandleEvent(pEvent);
	}
}

void GameEventManager::QueueEvent(IEvent * pEvent)
{
	event_queue.push(pEvent);
}

void GameEventManager::Tick(void)
{
	std::vector<IEventListener*> * vec;
	while (event_queue.size()) {
		vec = &lsnrMap[event_queue.front()->GetType().iType];
		for (int i=0;i<vec->size();++i) {
			vec->at(i)->HandleEvent(event_queue.front());
		}
		delete event_queue.front();
		event_queue.pop();
	}
}