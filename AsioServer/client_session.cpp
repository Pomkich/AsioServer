#include "client_session.h"

shared_ptr<client_session> client_session::Create(io_context& con, Server* sprt, int new_id) {
	shared_ptr<client_session> new_client_ses(new client_session(con, sprt, new_id));
	return new_client_ses;
}

void client_session::Start() {
	cout << "client session started, waiting for message..." << endl;
	Write("you're welcome!\n");
	sock.async_receive(buffer(read_buf), std::bind(&client_session::OnRead, shared_from_this()));
}

void client_session::Write(string mes) {
	std::copy(mes.begin(), mes.end(), write_buf);
	sock.async_send(buffer(write_buf, mes.size()), std::bind(&client_session::OnWrite, shared_from_this()));
}

ip::tcp::socket& client_session::GetSocket() { return sock; }
int client_session::GetSessionId() { return session_id; }

client_session::~client_session() { sock.close(); delete serv_prt; }
client_session::client_session(io_context& con, Server* sprt, int new_id) : sock(con), serv_prt(sprt), session_id(new_id) {};

void client_session::Read() {
	sock.async_receive(buffer(read_buf), std::bind(&client_session::OnRead, shared_from_this()));
}

void client_session::OnWrite() {
	cout << "message was send" << endl;
}

void client_session::OnRead() {
	cout << "message received: " << read_buf << endl;
	HandleMessage();
	memset(read_buf, 0x00, 1024);
	Read();
}

void client_session::HandleMessage() {
	string message;
	stringstream ss(read_buf);
	getline(ss, message, '\n');

	// удаляем пробелы с начала сообщения
	auto iter = find_if(message.begin(), message.end(), [](char c) { return (c != ' '); });
	if (iter != message.begin()) {
		message.erase(remove(message.begin(), iter, ' '), iter);
	}
	// обработка сообшений
	if (message == "-getclnts") {
		serv_prt->SendClientsList(session_id);	//
	}
	else if (message == "-disconect") {
		serv_prt->CloseSession(session_id);
	}
	else {
		// Server.SendNewMessage(session_id, message);
	}
}