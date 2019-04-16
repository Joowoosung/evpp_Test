#include "NT_MANAGER_CLIENT.h"
#include <string.h>
#include <memory>

int combinations[10][5] = {
	{0,0,1,1,1},
	{0,1,0,1,1},
	{1,0,0,1,1},
	{0,1,1,0,1},
	{0,1,1,1,0},
	{1,0,0,1,1},
	{1,0,1,0,1},
	{1,1,0,0,1},
	{1,1,0,1,0},
	{1,1,1,0,0}
};

void nt_manager::SelectNodes(std::list<CLIENT*>* list_node)
{
	int index(0);
	int x = std::rand() / ((RAND_MAX + 1u) / 10);

	if (_map_peer.size() <= 3)
	{
		for (auto itr : _map_peer)	list_node->push_back(itr.second);
		return;
	}

	//std::cout << "_map_peer size : " << _map_peer.size() << std::endl;

	for (auto itr : _map_peer)
	{
		//std::cout << "index: " << index << std::endl;
		if (combinations[x][index++] == 1)	list_node->push_back(itr.second);
	}
}

nt_manager::nt_manager()
{
}

nt_manager::~nt_manager()
{
	DisconnectAll();
}

void nt_manager::Connect_2_Peer(uint8_t remote_id, std::string remote_addr, CB_RECV_RES cb_res_msg_peer)
{
	CLIENT *Client = new CLIENT;
	Client->id = remote_id;
	Client->loop = new evpp::EventLoopThread;
	Client->client = std::make_shared<NT_TCPClient>(_id, remote_id, remote_addr, Client->loop->loop(), cb_res_msg_peer);
	Client->client->Connect();

	_map_peer.insert(std::make_pair<>(remote_id, Client));
	Client->loop->Start();


	std::cout << "Connect_2_Peer : _map_peer size : " << _map_peer.size() << std::endl;
}

void nt_manager::Connect_2_Order(uint8_t remote_id, std::string remote_addr, CB_RECV_RES cb_res_msg_orderer)
{
	CLIENT *Client = new CLIENT;
	Client->id = remote_id;
	Client->loop = new evpp::EventLoopThread;
	Client->client = std::make_shared<NT_TCPClient>(_id, remote_id, remote_addr, Client->loop->loop(), cb_res_msg_orderer);
	Client->client->Connect();

	_map_orderer.insert(std::make_pair<>(remote_id, Client));
	Client->loop->Start();

}

void nt_manager::DisconnectAll()
{
	for (auto itr : _map_peer)
	{
		itr.second->client->Disconnect();
		itr.second->loop->Stop();
	}
	_map_peer.clear();
	for (auto itr : _map_orderer)
	{
		itr.second->client->Disconnect();
		itr.second->loop->Stop();
	}
	_map_orderer.clear();
}

void nt_manager::SendMsg_2_peer_10m(sNT_MSG_10M* msg)
{
	SPTR_NT_TCPCLIENT client;
	std::list<CLIENT*> list_node;

	nt_msg_pool* pPool = nt_msg_pool::GetInstance();
	sNT_MSG* pmsg = pPool->GetMsg();

	uint64_t filesize = msg->data_len;


	// START File Send
	pmsg->type = NT_FILE_FIRST;
	pmsg->trx_id = msg->trx_id;
	pmsg->node_id = _id;
	pmsg->data_len = sizeof(uint64_t);
	memcpy(pmsg->buf, &filesize, sizeof(uint64_t));


	for (auto c : _map_peer)
	{
		if (c.second->client != nullptr)
		{
			c.second->client->SendMessage(pmsg);
#ifdef _TEST_BLOCK_
			printf("Send NT_FILE_FIRST : block size = %d\n", filesize);
#endif //_TEST_BLOCK_

		}
	}

	// SEND File
	uint8_t* pf = msg->buf;
	int send_byte(0);
	while (filesize > 0)
	{
		pmsg->type = NT_FILE_BLOCK;
		pmsg->data_len = (filesize > NT_MSG_BUF_SIZE) ? NT_MSG_BUF_SIZE : filesize;
		memcpy(pmsg->buf, pf, pmsg->data_len);
		pf += pmsg->data_len;
		filesize -= pmsg->data_len;

		for (auto c : _map_peer)
		{
			if (c.second->client != nullptr)
			{
				c.second->client->SendMessage(pmsg);
			}
		}
	}

	// End File Send
	pmsg->type = NT_FILE_END;
	pmsg->data_len = 0;

	for (auto c : _map_peer)
	{
		if (c.second->client != nullptr)
		{
			c.second->client->SendMessage(pmsg);
#ifdef _TEST_BLOCK_
			printf("Send NT_FILE_END\n");
#endif //_TEST_BLOCK_
		}
	}

	pPool->ReleaseMsg(pmsg);
	nt_msg_pool_10m::GetInstance()->ReleaseMsg(msg);
}


void nt_manager::SendMsg_2_peer(sNT_MSG* msg)
{
	SPTR_NT_TCPCLIENT client;
	std::list<CLIENT*> list_node;

	// random select 3 connection
	SelectNodes(&list_node);

	// send message 
	for (auto c : list_node)
	{
		if (c->client != nullptr)		c->client->SendMessage(msg);
	}

	nt_msg_pool::GetInstance()->ReleaseMsg(msg);
	list_node.clear();
}

void nt_manager::SendMsg_2_orderer(sNT_MSG* msg)
{
	if (_map_orderer.size() == 0)	return;

	int index(0);
	int x = std::rand() / ((RAND_MAX + 1u) / _map_orderer.size());

	for (auto itr : _map_orderer)
	{
		if (index++ == x)
		{
			itr.second->client->SendMessage(msg);
			break;
		}
	}

	nt_msg_pool::GetInstance()->ReleaseMsg(msg);
}
