#pragma once
#include <functional>

// #define _TEST_
//#define _TEST_BLOCK_


struct sNT_HEADER
{
	uint8_t src_id;
	int8_t msg_type;
	uint16_t msg_size;
	uint32_t trx_uid;
};
#define NT_HEAD_SIZE	8

#define NT_MSG_BUF_SIZE		1024
#define NT_MSG_BUF_SIZE_10M		10*1024*1024


#define NT_MSGTYPE_RECVABLE		-3
#define NT_MSGTYPE_BUSY			-2
#define NT_MSGTYPE_NODE_ID		-1

#define NT_MSGTYPE_REQ		1
#define NT_MSGTYPE_RES		2//

#define NT_FILE_FIRST		10
#define NT_FILE_BLOCK		11
#define NT_FILE_END			12
#define NT_FILE_ACK			13

