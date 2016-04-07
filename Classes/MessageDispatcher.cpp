#include "MessageDispatcher.h"


MessageDispatcher* MessageDispatcher::s_instance = 0;

MessageDispatcher::MessageDispatcher()
{

}

MessageDispatcher::~MessageDispatcher()
{

}

MessageDispatcher* MessageDispatcher::getInstance()
{
	if (!s_instance)
	{
		s_instance = new MessageDispatcher;
	}
	return s_instance;
}

void MessageDispatcher::pushMessage( unsigned int type, unsigned long param1 /*= 0*/, unsigned long param2 /*= 0*/ )
{
	_mutex.lock();
	Message* pMsg = new Message;
	pMsg->type = type;
	pMsg->param1 = param1;
	pMsg->param2 = param2;
	_msglist.push_back(pMsg);
	_mutex.unlock();
}

bool MessageDispatcher::popMessage( Message& Msg )
{
	_mutex.lock();
	bool ret = false;
	if (!_msglist.empty())
	{
		Message* pMsg = _msglist.front();
		_msglist.pop_front();
		Msg.type = pMsg->type;
		Msg.param1 = pMsg->param1;
		Msg.param2 = pMsg->param2;
		delete pMsg;
		ret = true;
	}
	_mutex.unlock();
	return ret;
}

