#pragma once

#include <thread>
#include <string>

#include "../nt_lib/NT_MANAGER_SVR.h"

class bussiness_logic
{
public:
	bussiness_logic(uint8_t id, std::string addr);
	virtual ~bussiness_logic();

	void Start();
	void Stop();

	void OnMessage(sNT_MSG* msg)
	{
		//
		if (_countMsg == 0) {
			//std::cout << "START send data[SERVER MESSAGE] [n:" << _countMsg << "]" << std::endl;
			start_time = std::chrono::system_clock::now().time_since_epoch().count();
		}
		_countMsg++;
		if (_countMsg > 200000)
		{
			end_time = std::chrono::system_clock::now().time_since_epoch().count();

			// <<< 대체 코드
			time_sub = (long double)(end_time - start_time) / 1000000000;
			
			std::cout.precision(10);
			std::lock_guard<std::mutex> guard(_mutex);
			std::cout << "200000 Duration[id :" << (int)_id << "] sec : " << time_sub << std::endl;
			// >>>

			// <<< 평균 시간 측정
			//if (time_sub < 1000) {
			//	_countAvg++;
			//	_time_sum += time_sub;
			//}
			//
			//if (_countAvg != 0) {
			//	std::cout << "200000 Duration [id :" << (int)_id << "] -------------  AVERAGE TIME : " << _time_sum / _countAvg << std::endl;
			//}
			// >>>

			// 대체 삭제
			//std::cout << "200000 Duration[id :" << (int)_id << "]" << (end_time - start_time) / 1000000000 << "." << (end_time - start_time) % 1000000000 << std::endl;

			std::cout << std::endl;
			_countMsg = -20000;
		}
		//
 		msg->type = NT_MSGTYPE_RES;
		_pNTMgr_svr->SendResponse(msg);
		
	}
	void OnMessage_10m(sNT_MSG_10M* msg)
	{

		msg->type = NT_MSGTYPE_RES;
//		_pNTMgr_svr->SendResponse(msg);
	}
private:
	uint8_t _id;

	nt_manager_svr* _pNTMgr_svr;
	nt_msg_pool* _msg_pool;


	std::list<int> _listCount;
	int _countMsg;
	int _countAvg;
	long double _time_sum = 0;
	long int start_time;
	long int end_time;
	long double time_sub;
	std::mutex _mutex;
};
