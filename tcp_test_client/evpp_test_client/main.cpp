#include <cstdio>
#include <string>
#include <vector>

#include "../nt_lib/NT_MANAGER_CLIENT.h"
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
	if (argc != 2)
	{
		printf("Sorry! Need Node-ID and Remote-ID!\n");
		return 0;
	}

	//
	init_node_info();

	// 
	uint8_t _id;
	_id = (uint8_t)atoi(argv[1]);

	auto itr = g_map_node_info.find(_id);
	if (itr == g_map_node_info.end())
	{
		printf("Sorry! Cannot find node_id info [node_id : %d]", _id);
		return 0;
	}


	bussiness_logic biz(_id);
	//biz.start();

	// <<< 대체 삭제 예정
	std::thread thrd = std::thread([&]() {biz.Start(); });
	//std::thread thrd_10m = std::thread([&]() {biz.Start_10m(); });
	// >>>



	// Network Init
	nt_manager* pMgr = nt_manager::GetInstance();
	pMgr->Init(_id);


	// connect to peers
	pMgr->Connect_2_Peer(11, g_map_node_info[11], std::bind(&bussiness_logic::OnMessage, &biz, std::placeholders::_1));
	pMgr->Connect_2_Peer(12, g_map_node_info[12], std::bind(&bussiness_logic::OnMessage, &biz, std::placeholders::_1));
	pMgr->Connect_2_Peer(13, g_map_node_info[13], std::bind(&bussiness_logic::OnMessage, &biz, std::placeholders::_1));
	pMgr->Connect_2_Peer(14, g_map_node_info[14], std::bind(&bussiness_logic::OnMessage, &biz, std::placeholders::_1));
	pMgr->Connect_2_Peer(15, g_map_node_info[15], std::bind(&bussiness_logic::OnMessage, &biz, std::placeholders::_1));
//	pMgr->ConnectTo(12, g_map_node_info[12]);
//	pMgr->ConnectTo(13, g_map_node_info[13]);
//	pMgr->ConnectTo(14, g_map_node_info[14]);
//	pMgr->ConnectTo(15, g_map_node_info[15]);



	while(1)	sleep(2);
	return 0;
}