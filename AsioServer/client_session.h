#pragma once
#include <iostream>
#include <asio.hpp>
#include "Server.h"

using namespace std;
using namespace asio;

class Server;

class client_session : public enable_shared_from_this<client_session> {
private:
	error_code err;
	Server* serv_ptr;
	int session_id;
	ip::tcp::socket sock;
	enum { max_buf = 1024 };
	char read_buf[max_buf];
	char write_buf[max_buf];
	bool started;

public:
	static shared_ptr<client_session> Create(io_context& con, Server* sprt, int new_id);

	void Start();
	void Stop();
	void Write(string mes);

	ip::tcp::socket& GetSocket();
	int GetSessionId();
	~client_session();

private:
	client_session(io_context& con, Server* sprt, int new_id);
	void Read();
	void OnWrite();
	void OnRead();
	void HandleMessage();
};