#include "NT_MSG_Pool.h"


////////////////////////////////////////////////////////////////////////////////
nt_msg_pool::nt_msg_pool()
{
	_count_msg = 0;
}

nt_msg_pool::~nt_msg_pool()
{
	Clear();
}

size_t nt_msg_pool::SetSize(size_t sz)
{
	sNT_MSG* msg;

	std::lock_guard<std::mutex> lg(_mtx);
	while (_list_msg.size() > sz)
	{
		msg = _list_msg.back();
		_list_msg.pop_back();
		--(_count_msg);

		delete msg;
	}

	while (_list_msg.size() < sz)
	{
		msg = new sNT_MSG;
		_list_msg.push_back(msg);
		++(_count_msg);
	}

	return _list_msg.size();
}

void nt_msg_pool::Clear()
{
	std::lock_guard<std::mutex> lg(_mtx);
	while (_list_msg.size() > 0)
	{
		delete _list_msg.front();
		_list_msg.pop_front();
	}
}

sNT_MSG* nt_msg_pool::GetMsg()
{
	sNT_MSG* msg;
	std::lock_guard<std::mutex> lg(_mtx);

	if (_list_msg.size() > 1)
	{
		msg = _list_msg.front();
		_list_msg.pop_front();
	}
	else
	{
		msg = new sNT_MSG;

		msg->type = 0;
		msg->trx_id = 0;
		msg->node_id = 0;
		msg->data_len = 0;

		++_count_msg;
	}

	return msg;
}

void nt_msg_pool::ReleaseMsg(sNT_MSG* msg)
{
	if (msg == nullptr)		return;

	msg->type = -1;
	msg->trx_id = 0;
	msg->node_id = 0;
	msg->data_len = 0;

	std::lock_guard<std::mutex> lg(_mtx);
	_list_msg.push_back(msg);
}