#pragma once
#include <string>
#include <functional>
#include <list>
#include <map>

#include "NT_MSG_Pool.h"
#include "NT_TCPClient.h"
#include <evpp/event_loop_thread.h>
#include <evpp/event_loop_thread.h>

struct CLIENT
{
	uint8_t id;
	SPTR_NT_TCPCLIENT client;
	evpp::EventLoopThread* loop;
};
typedef std::function<void(sNT_MSG*)>	CB_RECV_RES;
typedef std::function<void(sNT_MSG_10M*)>	CB_RECV_RES_10M;

class nt_manager
{
public:
	static nt_manager* GetInstance()
	{
		static nt_manager* _pInstance = new nt_manager;
		return _pInstance;
	}
	virtual ~nt_manager();

	void Init(uint8_t node_id)
	{
		_id = node_id;

		_pPool = nt_msg_pool::GetInstance();
		_pPool->SetSize(1000000);
		_pPool_10m = nt_msg_pool_10m::GetInstance();
		_pPool_10m->SetSize(100);
	}

	//////////////////////
	// Application Interface
	void Connect_2_Peer(uint8_t remote_id, std::string remote_addr, CB_RECV_RES cb_res_msg_peer);
	void Connect_2_Order(uint8_t remote_id, std::string remote_addr, CB_RECV_RES cb_res_msg_orderer);
	void DisconnectAll();

	void SendMsg_2_peer_10m(sNT_MSG_10M* msg);
	void SendMsg_2_peer(sNT_MSG* msg);
	void SendMsg_2_orderer(sNT_MSG* msg);

private:
	nt_manager();
	uint8_t _id;

	// Clients
	std::mutex _mtx_map_client;
	std::map<uint8_t, CLIENT*> _map_peer;
	std::map<uint8_t, CLIENT*> _map_orderer;



	evpp::EventLoopThread _loop;
	nt_msg_pool* _pPool;
	nt_msg_pool_10m* _pPool_10m;

private:
	void SelectNodes(std::list<CLIENT*>* list_node);


#ifndef _TEST_
	std::list<int> _count_res2;
#endif // _TEST_

};
