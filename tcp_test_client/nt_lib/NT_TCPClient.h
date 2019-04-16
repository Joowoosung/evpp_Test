#pragma once
#include <functional>
#include <condition_variable>

#include "NT_TCPConn.h"

#include <evpp/tcp_client.h>
#include <evpp/tcp_conn.h>
#include <evpp/buffer.h>

class NT_TCPClient
{
public:
	NT_TCPClient(uint8_t my_id, uint8_t remote_id, std::string remote_addr, 
		evpp::EventLoop* loop, CB_RECV_MSG cb_recv_msg)
		: _evpp_client(loop, remote_addr, "TCPClient")
	{
		_id = my_id;
		_remote_id = remote_id;
		_remote_addr = remote_addr;

		_nt_tcp_conn = nullptr;
		CB_recv_msg = cb_recv_msg;

		_evpp_client.SetMessageCallback(std::bind(&NT_TCPClient::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
		_evpp_client.SetConnectionCallback(std::bind(&NT_TCPClient::OnConnection, this, std::placeholders::_1));

#ifdef _TEST_
		_count_res.clear();
		nRecvCount = 0;
#endif // _TEST_
	}

	virtual ~NT_TCPClient()
	{
	}

	void Connect()	{
		_msg_pool = nt_msg_pool::GetInstance();
		_evpp_client.Connect();
	}
	void Disconnect()
	{
		_evpp_client.Disconnect();
		_nt_tcp_conn = nullptr;
	}

	// msg read / write
	void OnConnection(const evpp::TCPConnPtr& conn);
	void OnMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* msg);
	void OnControlMsg(const evpp::TCPConnPtr& conn, sNT_MSG* msg);
	void SendMessage(sNT_MSG* msg);

private:
	uint8_t _id;
	uint8_t _remote_id;
	std::string _remote_addr;

	// evpp
	evpp::TCPClient _evpp_client;
	SPTR_NT_TCPCONN _nt_tcp_conn;
	CB_RECV_MSG CB_recv_msg;

	// msg pool
	nt_msg_pool* _msg_pool;

#ifdef _TEST_
	std::list<int> _count_res;
	int nRecvCount;
#endif // _TEST_
};

typedef std::shared_ptr<NT_TCPClient> SPTR_NT_TCPCLIENT;