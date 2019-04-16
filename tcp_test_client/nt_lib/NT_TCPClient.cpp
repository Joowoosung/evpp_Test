#include "NT_TCPClient.h"

void NT_TCPClient::OnConnection(const evpp::TCPConnPtr& conn)
{
	if (conn->IsConnected())
	{
		_nt_tcp_conn = std::make_shared<NT_TCPConn>(conn, _id, 
			std::bind(&NT_TCPClient::OnControlMsg, this, std::placeholders::_1, std::placeholders::_2),
			CB_recv_msg, nullptr
		);

		sNT_MSG* msg = _msg_pool->GetMsg();
		msg->type = NT_MSGTYPE_NODE_ID;
		msg->trx_id = 0;
		msg->node_id = _id;
		msg->data_len = 0;
		SendMessage(msg);
		_msg_pool->ReleaseMsg(msg);

#ifdef _TEST_
		_count_res.clear();
		nRecvCount = 0;
#endif // _TEST_
		std::cout << "Connected to Server ... addr[" << conn->remote_addr() << "]" << std::endl;
	}
	else
	{
		_nt_tcp_conn = nullptr;
			std::cout << "DIsconnected to Server ... addr[" << conn->remote_addr() << "]" << std::endl;
	}
}

void NT_TCPClient::OnMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* msg)
{
	if (_nt_tcp_conn.get() != nullptr)
		_nt_tcp_conn->OnMessage(msg);
}

void NT_TCPClient::SendMessage(sNT_MSG* msg)
{
	msg->node_id = _id;
	if (_nt_tcp_conn.get() != nullptr)
		_nt_tcp_conn->SendMessage(msg);
}

void NT_TCPClient::OnControlMsg(const evpp::TCPConnPtr& conn, sNT_MSG* msg)
{
	switch (msg->type)
	{
	case NT_MSGTYPE_NODE_ID:
		break;
	}
	_msg_pool->ReleaseMsg(msg);
}
