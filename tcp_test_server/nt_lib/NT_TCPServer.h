#pragma once

#include "NT_DEFINE.h"
#include "NT_MSG_Pool.h"
#include "NT_TCPConn.h"

#include <evpp/tcp_server.h>
#include <evpp/tcp_conn.h>
#include <evpp/buffer.h>

#include <functional>
#include <condition_variable>

#include "NT_MSG_Pool.h"

#define EVPP_THREAD_NUM	10

class NT_TCPServer
{
public:
	NT_TCPServer(uint8_t node_id, evpp::EventLoop* loop, const std::string& svr_addr, CB_RECV_MSG cb_recv_msg, CB_RECV_MSG_10M cb_recv_msg_10m)
		: _tcp_svr(loop, svr_addr, "evppTCPServer", EVPP_THREAD_NUM)
	{
#ifdef _TEST_
		tcount = 0;
#endif // _TEST_

		_id = node_id;
		CB_recv_msg = cb_recv_msg;
		CB_recv_msg_10 = cb_recv_msg_10m;

		_tcp_svr.SetMessageCallback(std::bind(&NT_TCPServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
		_tcp_svr.SetConnectionCallback(std::bind(&NT_TCPServer::OnConnection, this, std::placeholders::_1));
	}

	virtual ~NT_TCPServer() {}

	void Start()
	{
		_msg_pool = nt_msg_pool::GetInstance();

		_tcp_svr.Init();
		_tcp_svr.Start();
	}

	void SendMessage(sNT_MSG* msg);
	SPTR_NT_TCPCONN GetConn(uint8_t remote_id)
	{
		SPTR_NT_TCPCONN pConn(nullptr);
		auto itr = _map_id.find(remote_id);
		if (itr != _map_id.end())
			pConn = itr->second;

		return pConn;
	}

private:
	void OnMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* msg);
	void OnConnection(const evpp::TCPConnPtr& conn);

private:
	uint8_t _id;
	evpp::TCPServer _tcp_svr;

	// conn map
	std::mutex _mtx_map_connect;
//	std::map<evpp::TCPConnPtr, SPTR_NT_TCPCONN> _map_conn;SPTR_TCPPEER
//	std::map<uint8_t, SPTR_NT_TCPCONN> _map_id;
	std::map<evpp::TCPConnPtr, SPTR_NT_TCPCONN> _map_conn;
	std::map<uint8_t, SPTR_NT_TCPCONN> _map_id;

	// for connection
	CB_RECV_MSG CB_recv_msg;
	CB_RECV_MSG_10M CB_recv_msg_10;
	nt_msg_pool* _msg_pool;

	void signal_conn(const evpp::TCPConnPtr& conn, sNT_MSG* msg)
	{
		switch (msg->type)
		{
		case NT_MSGTYPE_NODE_ID:
			break;
		}
		_msg_pool->ReleaseMsg(msg);
	}

public:
	// node map
	void SetNodeInfo(std::map<uint8_t, std::string>* map_node_info)
	{
		_map_node_info = map_node_info;
	}
	std::map<uint8_t, std::string>* _map_node_info;



	// 10m 통신용






	// test
	int tcount;
};
