#include <iostream>
#include "bussiness_logic.h"


///////////////////////////////////////////

bussiness_logic::bussiness_logic(uint8_t id, std::string addr)
{
	_id = id;
	_pNTMgr_svr = nt_manager_svr::GetInstance();
}


bussiness_logic::~bussiness_logic()
{
}

void bussiness_logic::Start()
{
	_msg_pool = nt_msg_pool::GetInstance();

	while (1)
	{
		sleep(5);
	}
}

void bussiness_logic::Stop()
{

}
