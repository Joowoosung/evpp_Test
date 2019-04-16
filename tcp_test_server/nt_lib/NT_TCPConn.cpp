#include "NT_TCPConn.h"

void NT_TCPConn::OnMessage(evpp::Buffer* msg)
{
	std::lock_guard<std::mutex> lg(_mtx_r);
	evpp::Slice s;
	int need_len;

#ifdef _TEST_
	int test_len(0);

#endif // _TEST_
	do {
#ifdef _TEST_
		if (_r_len > 1024)
		{
			std::cout << std::endl << std::endl << "Recv ... _r_len : " << std::endl;
		}
		test_len = msg->length();
#endif // _TEST_
		if (_r_len < NT_HEAD_SIZE)
		{
			need_len = NT_HEAD_SIZE - _r_len;
			s = msg->Next(need_len);
			memcpy(_r_buf + _r_len, s.data(), (int)s.size());
			_r_len += (int)s.size();
		}
		if (_r_len < NT_HEAD_SIZE)	return;

#ifdef _TEST_
		test_len = msg->length();
#endif // _TEST_

		sNT_HEADER* hdr = (sNT_HEADER*)_r_buf;
		need_len = NT_HEAD_SIZE + hdr->msg_size - _r_len;
		s = msg->Next(need_len);
		memcpy(_r_buf + _r_len, s.data(), (int)s.size());
		_r_len += (int)s.size();

		if (_r_len == (NT_HEAD_SIZE + hdr->msg_size))
		{
			sNT_MSG* pmsg = _pPool->GetMsg();
			pmsg->type = hdr->msg_type;
			pmsg->trx_id = hdr->trx_uid;
			pmsg->node_id = hdr->src_id;
			pmsg->data_len = hdr->msg_size;
			memcpy(pmsg->buf, _r_buf + NT_HEAD_SIZE, hdr->msg_size);
			_r_len = 0;
#ifdef _TEST_BLOCK_
			printf("NT_TCPConn::OnMessage : msg_type = %d\n", pmsg->type);
#endif
			if (pmsg->type == NT_MSGTYPE_REQ || pmsg->type == NT_MSGTYPE_RES)
			{
				if (_cb_recv != nullptr)	_cb_recv(pmsg);
				else						_pPool->ReleaseMsg(pmsg);
			}
			else if (pmsg->type == NT_FILE_FIRST || pmsg->type == NT_FILE_BLOCK || pmsg->type == NT_FILE_END)
			{
	#ifdef _TEST_BLOCK_
				printf("NT_TCPConn::OnMessage2 : msg_type = %d\n", pmsg->type);
	#endif
				Recv_file(pmsg);
			}
			else
			{
				if (_cb_ctrl != nullptr)	_cb_ctrl(_conn, pmsg);
				else						_pPool->ReleaseMsg(pmsg);
			}
		}
	} while (msg->length() > 0);
}

void NT_TCPConn::SendMessage(sNT_MSG* msg)
{
	std::lock_guard<std::mutex> lg(_mtx_s);
	sNT_HEADER* hdr = (sNT_HEADER*)_s_buf;
	hdr->trx_uid = msg->trx_id;
	hdr->src_id = msg->node_id;
	hdr->msg_type = msg->type;
	hdr->msg_size = (int16_t)msg->data_len;
	memcpy(_s_buf + NT_HEAD_SIZE, msg->buf, hdr->msg_size);

	_conn->Send(_s_buf, NT_HEAD_SIZE + hdr->msg_size);
}
