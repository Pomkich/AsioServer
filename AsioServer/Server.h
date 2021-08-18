#pragma once
#include <iostream>
#include <asio.hpp>
#include "client_session.h"

using namespace std;
using namespace asio;

class client_session;

class Server {
private:
	int id_iter = 0;
	io_context context;
	ip::tcp::acceptor acceptor;
	list<shared_ptr<client_session>> clients;

public:
	Server(ip::tcp::endpoint ep);

	void Run();
	void SendClientsList(int session_id);
	void CloseSession(int session_id);
	void SendNewMessage(int session_id, string message);

private:
	void AcceptHandler(std::shared_ptr<client_session> clt, error_code& err);
};