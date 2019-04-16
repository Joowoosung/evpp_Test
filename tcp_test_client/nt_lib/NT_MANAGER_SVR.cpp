#include "NT_MANAGER_SVR.h"
#include <string.h>

nt_manager_svr::nt_manager_svr()
{
	_tcpserver = nullptr;
}

nt_manager_svr::~nt_manager_svr()
{
	Stop();
}

void nt_manager_svr::Start()
{
	_bStop = false;

	_thrd_pool.resize(WORK_THREAD_COUNT);

	for (int i = 0; i < WORK_THREAD_COUNT; i++)
	{
		_thrd_pool[i] = std::thread([&]() { work_thread(); });
	}
	_thrd_work_10m = std::thread([&]() { work_thread_10m(); });


	_tcpserver = new NT_TCPServer(_id, _loop.loop(), _addr,
		std::bind(&nt_manager_svr::OnMessage, this, std::placeholders::_1),
		std::bind(&nt_manager_svr::OnMessage_10m, this, std::placeholders::_1)
	);
	_tcpserver->SetNodeInfo(&_map_node);
	_tcpserver->Start();
	_loop.Start();
}

void nt_manager_svr::Stop()
{
	_loop.Stop(true);

	_bStop = true;
	_cond_work.notify_all();
	for (int i = 0; i < WORK_THREAD_COUNT; i++)
	{
		if (_thrd_pool[i].joinable())	_thrd_pool[i].join();
	}

	if (_thrd_work_10m.joinable())	_thrd_work_10m.join();
}

// map node info
void nt_manager_svr::AddNodeInfo(std::map<uint8_t, std::string>* map_node)
{
	std::string addr;
	size_t pos;

	for (auto itr : *map_node)
	{
		addr = itr.second;
		pos = addr.find(":");
		if (pos != std::string::npos)			addr = addr.substr(0, pos) + ":0";
		else									addr = addr + ":0";

		_map_node.insert(std::make_pair(itr.first, addr));
	}
}
void nt_manager_svr::AddNodeInfo(uint8_t id, std::string ip, int port)
{
	_map_node.insert(std::make_pair(id, ip + ":" + "0"/*std::to_string(port)*/));
}
