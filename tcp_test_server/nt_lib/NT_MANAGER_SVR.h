#pragma once
#include <string>
#include <functional>
#include <list>
#include <map>

#include "NT_MSG_Pool.h"
#include "NT_TCPServer.h"

#define WORK_THREAD_COUNT 10

typedef std::function<void(sNT_MSG_10M*)>	CB_RECV_MSG_10M;

class nt_manager_svr
{
public:
	static nt_manager_svr* GetInstance()
	{
		static nt_manager_svr* _pInstance = new nt_manager_svr;
		return _pInstance;
		
	}
	virtual ~nt_manager_svr();

	// Control Interface
	void SetServerInfo(uint8_t node_id, std::string addr, 
		CB_RECV_MSG cb_recv_msg, CB_RECV_MSG_10M cb_recv_msg_10m)
	{
		std::string t = addr;
		t = addr.substr(addr.find(":") + 1);
		_addr = "0.0.0.0:";

		_id = node_id;
		_addr += t;
		CB_recv_msg = cb_recv_msg;
		CB_recv_msg_10m = cb_recv_msg_10m;

		_pPool = nt_msg_pool::GetInstance();
		_pPool->SetSize(10000);
		_pPool_10m = nt_msg_pool_10m::GetInstance();
		_pPool_10m->SetSize(10);
	}

	void Start();
	void Stop();

	//////////////////////
	// Application Interface
	void AddNodeInfo(std::map<uint8_t, std::string>* map_node);
	void AddNodeInfo(uint8_t id, std::string ip, int port);

	void SendMsg(sNT_MSG* msg)
	{
		SPTR_NT_TCPCONN conn = _tcpserver->GetConn(msg->node_id);
		if (conn.get() != nullptr)
			conn->SendMessage(msg);

		nt_msg_pool::GetInstance()->ReleaseMsg(msg);

	}
	void SendRequest(sNT_MSG* msg)
	{
		msg->type = NT_MSGTYPE_REQ;
		SendMsg(msg);
	}
	void SendResponse(sNT_MSG* msg)
	{
		msg->type = NT_MSGTYPE_RES;
		SendMsg(msg);
	}
	void SendResponse(sNT_MSG_10M* msg)
	{
		sNT_MSG* pmsg = nt_msg_pool::GetInstance()->GetMsg();
		pmsg->type = NT_FILE_ACK;
		pmsg->trx_id = msg->trx_id;
		pmsg->node_id = msg->node_id;
		pmsg->data_len = msg->data_len;
		SendMsg(pmsg);

		nt_msg_pool_10m::GetInstance()->ReleaseMsg(msg);
	}

private:	// work_thread
	// 1024
	void OnMessage(sNT_MSG* msg)
	{
		std::lock_guard<std::mutex> lg(_mtx_work);
		_que_work.push_back(msg);
		_cond_work.notify_all();
	}

	bool _bStop;
	std::mutex _mtx_work;
	std::condition_variable _cond_work;
	std::list<sNT_MSG*> _que_work;
	std::vector<std::thread> _thrd_pool;
	void work_thread()
	{
		sNT_MSG* work_data(nullptr);
		std::unique_lock<std::mutex> ul(_mtx_work);

		while (!_bStop)
		{
			if (_que_work.size() == 0)
				_cond_work.wait(ul);

			if (_que_work.size() > 0)
			{
				work_data = _que_work.front();
				_que_work.pop_front();
			}
			ul.unlock();

			if (work_data != nullptr)
			{
				CB_recv_msg(work_data);
				work_data = nullptr;
			}
			ul.lock();
		}
	}

	// 10M 
	void OnMessage_10m(sNT_MSG_10M* msg)
	{
		std::lock_guard<std::mutex> lg(_mtx_work_10m);
		_que_work_10m.push_back(msg);
		_cond_work_10m.notify_all();
	}

	std::mutex _mtx_work_10m;
	std::condition_variable _cond_work_10m;
	std::list<sNT_MSG_10M*> _que_work_10m;
	std::thread _thrd_work_10m;
	void work_thread_10m()
	{
		sNT_MSG_10M* work_data(nullptr);
		std::unique_lock<std::mutex> ul(_mtx_work_10m);

		while (!_bStop)
		{
			if (_que_work_10m.size() == 0)
				_cond_work_10m.wait(ul);

			if (_que_work_10m.size() > 0)
			{
				work_data = _que_work_10m.front();
				_que_work_10m.pop_front();
			}
			ul.unlock();

			if (work_data != nullptr)
			{
				CB_recv_msg_10m(work_data);
				this->_pPool_10m->ReleaseMsg(work_data);
				work_data = nullptr;
			}
			ul.lock();
		}
	}

private:
	nt_manager_svr();
	nt_msg_pool* _pPool;
	nt_msg_pool_10m* _pPool_10m;

	uint8_t _id;
	std::string _addr;
	CB_RECV_MSG CB_recv_msg;
	CB_RECV_MSG_10M CB_recv_msg_10m;

	// server
	evpp::EventLoopThread _loop;
	NT_TCPServer* _tcpserver;

	// map node info
	std::map<uint8_t, std::string> _map_node;
};
