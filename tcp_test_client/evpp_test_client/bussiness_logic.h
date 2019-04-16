#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

#include "../nt_lib/NT_MANAGER_CLIENT.h"

#define TEST_COUNT 200000

#define TEST_COUNT_10M 1

class bussiness_logic
{
public:
	bussiness_logic(uint8_t id);
	virtual ~bussiness_logic();

	void Start()
	{
		_pNTMgr_client = nt_manager::GetInstance();
		_msg_pool = nt_msg_pool::GetInstance();

		sleep(5);

		uint32_t count(TEST_COUNT);
		sNT_MSG* msg;

		while (1)
		{
			_list_recv_count.clear();
			std::cout << "START send data[TRANSACTION] [n:" << TEST_COUNT << "]" << std::endl;
			start_time = std::chrono::system_clock::now().time_since_epoch().count();
			start_time_sec = std::chrono::system_clock::now();
			for (uint32_t i = 1; i <= count; i++)
			{
				msg = _msg_pool->GetMsg();
				msg->type = NT_MSGTYPE_REQ;
				msg->trx_id = i;
				msg->node_id = 11;
				msg->data_len = 200;
				memset(msg->buf, 'A', msg->data_len);
				_pNTMgr_client->SendMsg_2_peer(msg);
			}
					end_time = std::chrono::system_clock::now().time_since_epoch().count();
					// <<< 대체 코드
					time_sub = (long double)(end_time - start_time) / 1000000000;
					std::cout.precision(10);
					std::cout <<"[id : "<< (int)_id <<"] end time[TRANSACTION] : " << time_sub << std::endl;
					// >>>

					// 대체 삭제
					//std::cout << "[id : " << (int)_id << "] end time[TRANSACTION] : " << (end_time_10m - start_time_10m) / 1000000000 << "." << (end_time_10m - start_time_10m) % 1000000000 << std::endl;
					
					std::cout << std::endl << std::endl << "Send End[TRANSACTION]..." << std::endl << std::endl;
			//getchar();
			usleep(1);
		}
	}
	void Stop() {}

	void Start_10m()
	{
		_pNTMgr_client = nt_manager::GetInstance();
		_msg_pool_10m = nt_msg_pool_10m::GetInstance();

		sleep(5);

		uint32_t count(TEST_COUNT_10M);
		sNT_MSG_10M* msg;

		while (1)
		{
			

			std::cout << "START send data[BLOCK DATA] [n:" << TEST_COUNT_10M << "]" << std::endl;
			start_time_10m = std::chrono::system_clock::now().time_since_epoch().count();
			for (uint32_t i = 1; i <= count; i++)
			{
				msg = _msg_pool_10m->GetMsg();
				msg->type = NT_MSGTYPE_REQ;
				msg->trx_id = i;
				msg->node_id = 11;
				msg->data_len = NT_MSG_BUF_SIZE_10M;
				memset(msg->buf, 'A', msg->data_len);
				_pNTMgr_client->SendMsg_2_peer_10m(msg);

			}
			end_time_10m = std::chrono::system_clock::now().time_since_epoch().count();
			// <<< 대체 코드
			time_sub = (long double)(end_time_10m - start_time_10m) / 1000000000;
			std::cout.precision(10);
			std::cout << "end time[BLOCK DATA] : " << time_sub << std::endl;
			// >>>


			// 대체 삭제
			//std::cout << "end time[BLOCK DATA] : " << (end_time_10m - start_time_10m) / 1000000000 << "." << (end_time_10m - start_time_10m) % 1000000000 << std::endl;

			std::cout << std::endl << std::endl << "Send End[BLOCK DATA]..." << std::endl << std::endl;
			//getchar();
			usleep(1);
		}
	}
	void Stop_10m() {}

	void OnMessage(sNT_MSG* msg)
	{
		_list_recv_count.push_back(msg->trx_id);

//		if (_list_recv_count.size() % 1000 == 0)
//			std::cout << "Recieve Respons : " << _list_recv_count.size() << std::endl;

		if (TEST_COUNT == _list_recv_count.size())
		{
			// <<< 수신완료 시간 측정
			/*std::cout << std::endl << "END recv data [n:" << TEST_COUNT << "]" << std::endl;
			std::cout << "Recv trx count : " << _list_recv_count.size() << std::endl;
			end_time = std::chrono::system_clock::now().time_since_epoch().count();
			time_sub = (long double)(end_time - start_time) / 1000000000;
			std::cout.precision(10);
			std::cout << "Recv end time[TRANSACTION] : " << time_sub << std::endl << std::endl << std::endl;*/
			// >>>


			// 대체 삭제
			//std::cout << "Recv end time[TRANSACTION] : " << (end_time - start_time) / 1000000000 << "." << (end_time - start_time) % 1000000000 << std::endl << std::endl << std::endl;
			

		}
	}

private:
	uint8_t _id;

	nt_manager* _pNTMgr_client;
	nt_msg_pool* _msg_pool;
	nt_msg_pool_10m* _msg_pool_10m;

	long int start_time;
	long int end_time;

	std::chrono::system_clock::time_point start_time_sec;
	//double end_time_sec;

	long int start_time_10m;
	long int end_time_10m;
	long double time_sub;

	std::list<int> _list_recv_count;
};
