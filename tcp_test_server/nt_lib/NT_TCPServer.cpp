
#include "NT_TCPServer.h"

void NT_TCPServer::OnConnection(const evpp::TCPConnPtr& conn)
{
	if (conn->IsConnected())
	{
		std::string addr = conn->remote_addr();
		std::string ip = addr.substr(0, addr.find(":")) + ":0";
		uint8_t remote_id = 0;
		for (auto itr : *_map_node_info)
		{
			if (ip.compare(itr.second) == 0)
			{
				remote_id = itr.first;
				break;
			}
		}
		if (remote_id != 0)
		{
			std::unique_lock<std::mutex> ul(_mtx_map_connect);
			auto itr = _map_id.find(remote_id);
			if (itr != _map_id.end())
			{	// disconnect exist connection
				std::cout << "CLOSE Exist Connection id[" << std::to_string(remote_id) << "], addr[" << itr->second->_remote_addr << "]" << std::endl;
				itr->second->Close();
				for (auto itr_2 : _map_conn)
				{
					if (itr_2.second == itr->second)
					{
						_map_conn.erase(itr_2.first);
						break;
					}
				}
				_map_id.erase(itr);
			}
			ul.unlock();

//			SPTR_NT_TCPCONN pConn =
//				std::make_shared<NT_TCPConn>(conn, remote_id,
//					std::bind(&NT_TCPServer::signal_conn, this, std::placeholders::_1, std::placeholders::_2),
//					CB_recv_msg);
			SPTR_NT_TCPCONN pConn =
				std::make_shared<NT_TCPConn>(conn, remote_id,
					std::bind(&NT_TCPServer::signal_conn, this, std::placeholders::_1, std::placeholders::_2),
					CB_recv_msg, CB_recv_msg_10);

			ul.lock();
			_map_conn.insert(std::make_pair(conn, pConn));
			_map_id.insert(std::make_pair(remote_id, pConn));

			std::cout << "Accepted Client ... id[" << std::to_string(remote_id) << "], addr[" << conn->remote_addr() <<"]" << std::endl;
		}
	}
	else
	{
		return;
		std::lock_guard<std::mutex> lg(_mtx_map_connect);
		auto itr = _map_conn.find(conn);
		if (itr != _map_conn.end())
		{
			_map_id.erase(itr->second->_remote_id);
			_map_conn.erase(conn);

			std::cout << "Bye Client ..." << std::endl;
		}
	}
}

void NT_TCPServer::OnMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* msg)
{
//	std::lock_guard<std::mutex> lg(_mtx_map_connect);
	auto itr = _map_conn.find(conn);
	if (itr != _map_conn.end())
	{
		itr->second->OnMessage(msg);
	}
}


void NT_TCPServer::SendMessage(sNT_MSG* msg)
{
	auto itr = _map_id.find(msg->node_id);
	if (itr != _map_id.end())
	{
		msg->node_id = _id;
		itr->second->SendMessage(msg);
	}
	else
	{
		_msg_pool->ReleaseMsg(msg);
	}
}
