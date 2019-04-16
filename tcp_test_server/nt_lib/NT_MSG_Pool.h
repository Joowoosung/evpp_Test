#pragma once

#include <stdint.h>
#include <list>
#include <mutex>

#include "NT_DEFINE.h"

struct sNT_MSG
{
	int8_t type;	// msg type
	uint8_t node_id;
	uint32_t trx_id;
	int data_len;
	uint8_t buf[NT_MSG_BUF_SIZE];
};

class nt_msg_pool
{
public:
	static nt_msg_pool* GetInstance()
	{
		static nt_msg_pool* _pInstance = new nt_msg_pool;
		return _pInstance;
	}

	~nt_msg_pool();

public:
	sNT_MSG* GetMsg();
	void ReleaseMsg(sNT_MSG* msg);
	void Clear();
	std::size_t SetSize(std::size_t sz);
	std::size_t GetSize() { return _list_msg.size(); }

private:
	nt_msg_pool();
	nt_msg_pool* _pInstance;

	std::list<sNT_MSG*> _list_msg;
	std::size_t _count_msg;

	std::mutex _mtx;
};

struct sNT_MSG_10M
{
	int8_t type;	// msg type
	uint8_t node_id;
	uint32_t trx_id;
	int data_len;
	uint8_t buf[NT_MSG_BUF_SIZE_10M];
};

class nt_msg_pool_10m
{
public:
	static nt_msg_pool_10m* GetInstance()
	{
		static nt_msg_pool_10m* _pInstance_10m = new nt_msg_pool_10m;
		return _pInstance_10m;
	}

	~nt_msg_pool_10m() { _list_msg.clear(); }

public:
	sNT_MSG_10M* GetMsg()
	{
		sNT_MSG_10M* msg;
		std::lock_guard<std::mutex> lg(_mtx);

		if (_list_msg.size() > 1)
		{
			msg = _list_msg.front();
			_list_msg.pop_front();
		}
		else
		{
			msg = new sNT_MSG_10M;

			msg->type = 0;
			msg->trx_id = 0;
			msg->node_id = 0;
			msg->data_len = 0;

			++_count_msg;
		}

		return msg;
	}

	void ReleaseMsg(sNT_MSG_10M* msg)
	{
		msg->type = -1;
		msg->trx_id = 0;
		msg->node_id = 0;
		msg->data_len = 0;

		std::lock_guard<std::mutex> lg(_mtx);
		_list_msg.push_back(msg);
	}

	void Clear()
	{
		std::lock_guard<std::mutex> lg(_mtx);
		while (_list_msg.size() > 0)
		{
			delete _list_msg.front();
			_list_msg.pop_front();
		}
	}

	std::size_t SetSize(std::size_t sz)
	{
		sNT_MSG_10M* msg;

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
			msg = new sNT_MSG_10M;
			_list_msg.push_back(msg);
			++(_count_msg);
		}

		return _list_msg.size();
	}
	std::size_t GetSize() { return _list_msg.size(); }

private:
	nt_msg_pool_10m() {	_count_msg = 0;	}
	nt_msg_pool_10m* _pInstance_10m;

	std::list<sNT_MSG_10M*> _list_msg;
	std::size_t _count_msg;

	std::mutex _mtx;
};
