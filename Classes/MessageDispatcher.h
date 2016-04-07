#ifndef _Message_Dispatcher_H_
#define _Message_Dispatcher_H_

#include <list>
#include <mutex>

struct Message
{
	unsigned int type;
	unsigned long param1;
	unsigned long param2;
};

class MessageDispatcher
{
	typedef std::list<Message*> MessageList;

public:
	static MessageDispatcher* getInstance();

	void pushMessage(unsigned int type, unsigned long param1 = 0, unsigned long param2 = 0);
	bool popMessage(Message& pMsg);

private:
	MessageDispatcher();
	~MessageDispatcher();

	static MessageDispatcher* s_instance;
	MessageList	_msglist;
	std::mutex _mutex;
};

#endif