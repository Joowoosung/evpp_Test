#pragma once

#include "NT_DEFINE.h"
#include "NT_MSG_Pool.h"

#include <evpp/tcp_conn.h>
#include <mutex>

class NT_TCPConn;
typedef std::function<void(sNT_MSG*)>	CB_RECV_MSG;
typedef std::function<void(sNT_MSG_10M*)>	CB_RECV_MSG_10M;
typedef std::function<void(const evpp::TCPConnPtr& conn, sNT_MSG*)>	CB_RECV_CTRL;

class NT_TCPConn
{
public:
	NT_TCPConn(const evpp::TCPConnPtr& conn, uint8_t remote_id, CB_RECV_CTRL cb_ctrl, CB_RECV_MSG cb_recv, CB_RECV_MSG_10M cb_recv_10m) :
		_conn(conn), _remote_id(remote_id), _remote_addr(conn->remote_addr())
	{
		_r_len = 0;
		_s_len = 0;

		CB_recv_10m = cb_recv_10m;
		_cb_recv = cb_recv;
		_cb_ctrl = cb_ctrl;

		_pPool = nt_msg_pool::GetInstance();

#ifdef _TEST_
		nResendCount = 0;
#endif // _TEST_
	}
	~NT_TCPConn() {}
	void Close()
	{
		if (_conn->IsConnected())
		{
			_conn->Close();
		}
		else
		{
			std::cout << "This is already Disconnected... remote_id[" << std::to_string(_remote_id) << "], remote_addr["<< _remote_addr <<"]"<< std::endl;
		}
	}
public:
	// recv
	void OnMessage(evpp::Buffer* msg);
	virtual void Recv_file(sNT_MSG* msg)
	{
		std::lock_guard<std::mutex> lg(_mtx_r_10m);

		if (msg->type == NT_FILE_FIRST)
		{
			if (_msg_10m == nullptr)
			{
				_msg_10m = nt_msg_pool_10m::GetInstance()->GetMsg();
			}
			_msg_10m->type = NT_FILE_FIRST;
			_msg_10m->trx_id = msg->trx_id;
			_msg_10m->node_id = msg->node_id;

			uint64_t sz;
			memcpy(&sz, msg->buf, sizeof(uint64_t));
			_msg_10m->data_len = (int)sz;
#ifdef _TEST_BLOCK_
			printf("recv NT_FILE_FIRST : block size = %d\n", _msg_10m->data_len);
#endif //_TEST_BLOCK_

			_r_len_10m = 0;

			nt_msg_pool::GetInstance()->ReleaseMsg(msg);
		}
		else if (msg->type == NT_FILE_BLOCK)
		{
			if (_msg_10m != nullptr)
			{
				memcpy(_msg_10m->buf + _r_len_10m, msg->buf, msg->data_len);
				_r_len_10m += msg->data_len;
			}
			nt_msg_pool::GetInstance()->ReleaseMsg(msg);
		}
		else if (msg->type == NT_FILE_END)
		{
#ifdef _TEST_BLOCK_
			printf("recv NT_FILE_END\n");
#endif //_TEST_BLOCK_

			if (_msg_10m != nullptr && CB_recv_10m != nullptr)
			{
#ifdef _TEST_BLOCK_
				printf("callback recv block\n");
#endif //_TEST_BLOCK_
				CB_recv_10m(_msg_10m);
				_msg_10m = nullptr;
			}
			else
			{
				nt_msg_pool_10m::GetInstance()->ReleaseMsg(_msg_10m);
				_msg_10m = nullptr;
			}
			
			msg->type = NT_FILE_ACK;
			msg->data_len = 0;

			SendMessage(msg);
			return;
		}
	}
	CB_RECV_MSG _cb_recv;
	CB_RECV_CTRL _cb_ctrl;

	// send
	void SendMessage(sNT_MSG* msg);

public:
	const uint8_t _remote_id;
	const std::string _remote_addr;

private:
	const evpp::TCPConnPtr _conn;

	// read msg
	std::mutex _mtx_r;
	int _r_len;
	uint8_t _r_buf[NT_HEAD_SIZE+NT_MSG_BUF_SIZE];

	// send msg
	std::mutex _mtx_s;
	int _s_len;
	uint8_t _s_buf[NT_HEAD_SIZE+NT_MSG_BUF_SIZE];

	// msg pool
	nt_msg_pool* _pPool;





	// read msg
	std::mutex _mtx_r_10m;
	int _r_len_10m;
	sNT_MSG_10M* _msg_10m;

	CB_RECV_MSG_10M CB_recv_10m;


#ifdef _TEST_
	int nResendCount;
#endif // _TEST
};

typedef std::shared_ptr<NT_TCPConn> SPTR_NT_TCPCONN;
