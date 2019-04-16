#include <cstdio>

#include "../nt_lib/NT_MANAGER_SVR.h"
#include "bussiness_logic.h"

std::map<uint8_t, std::string> g_map_node_info;
void init_node_info()
{
	// Client
	g_map_node_info.insert(std::make_pair<>(1, "54.180.95.1:8910"));
	g_map_node_info.insert(std::make_pair<>(2, "52.78.166.10:8920"));
	g_map_node_info.insert(std::make_pair<>(3, "13.125.98.107:8930"));
	g_map_node_info.insert(std::make_pair<>(4, "54.180.125.169:8940"));
	g_map_node_info.insert(std::make_pair<>(5, "54.180.120.222:8940"));

	// Peer
	g_map_node_info.insert(std::make_pair<>(11, "54.180.87.92:9010"));
	g_map_node_info.insert(std::make_pair<>(12, "54.180.87.92:9020"));
	g_map_node_info.insert(std::make_pair<>(13, "54.180.87.92:9030"));
	g_map_node_info.insert(std::make_pair<>(14, "54.180.87.92:9040"));
	g_map_node_info.insert(std::make_pair<>(15, "54.180.87.92:9050"));

	// Orderer
	g_map_node_info.insert(std::make_pair<>(23, "127.0.0.1:9510"));
	g_map_node_info.insert(std::make_pair<>(24, "127.0.0.1:9520"));
	g_map_node_info.insert(std::make_pair<>(25, "127.0.0.1:9530"));
}

int main(int argc, char* argv[])
{
	uint8_t _id;
	std::string port;
	init_node_info();

	if (argc != 2)
	{
		printf("Sorry! Need Node-ID and ListenPort!\n");
		return 0;
	}
	_id = (uint8_t)atoi(argv[1]);
	if (g_map_node_info.count(_id) == 0)
		return 0;


	// Bussiness 구동
	bussiness_logic biz(_id, g_map_node_info[_id]);

	// Tcp Server  구동
	nt_manager_svr* nt_mgr = nt_manager_svr::GetInstance();
	nt_mgr->AddNodeInfo(&g_map_node_info);
	nt_mgr->SetServerInfo(_id, g_map_node_info[_id], std::bind(&bussiness_logic::OnMessage, &biz, std::placeholders::_1), std::bind(&bussiness_logic::OnMessage_10m, &biz, std::placeholders::_1));
	nt_mgr->Start();

	printf("Server[%03d/%s] started...\n", _id, port.c_str());

	// Bussiness 구동
	biz.Start();

	return 0;
}

