#include "client_session.h"

shared_ptr<client_session> client_session::Create(io_context& con, Server* sprt, int new_id) {
	shared_ptr<client_session> new_client_ses(new client_session(con, sprt, new_id));
	return new_client_ses;
}

void client_session::Start() {
	started = true;
	cout << "client session started, waiting for login..." << endl;
	Write("[SERVER]: Waiting for login (<-login> <name>): \n");
	sock.async_receive(buffer(read_buf), std::bind(&client_session::OnRead, shared_from_this()));
}

void client_session::Stop() {
	if (!started) return;
	started = false;
	logined = false;
}

void client_session::Write(string mes) {
	if (!started) return;
	std::copy(mes.begin(), mes.end(), write_buf);
	sock.async_send(buffer(write_buf, mes.size()), std::bind(&client_session::OnWrite, shared_from_this()));
}

ip::tcp::socket& client_session::GetSocket() { return sock; }
int client_session::GetSessionId() { return session_id; }
string client_session::GetName() { return name; }

client_session::~client_session() {};
client_session::client_session(io_context& con, Server* sprt, int new_id) : sock(con), 
serv_ptr(sprt), session_id(new_id), started(false), logined(false), name("NULL") {};

void client_session::Read() {
	if (!started) return;
	sock.async_receive(buffer(read_buf), std::bind(&client_session::OnRead, shared_from_this()));
}

void client_session::OnWrite() {
	
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
	ss = stringstream(message);

	// обработка сообшений
	if (message == "-disconect") {
		serv_ptr->CloseSession(session_id);
	}
	if (!logined) {
		string com, nam;
		ss >> com >> nam;
		if (com == "-login") {
			logined = true;
			Write("logined, name: " + nam + '\n');
			name = nam;
		}
		else {
			Write("falied to login, please, try <-login> <name>");
		}
	}
	else {
		if (message == "-getclnts") {
			serv_ptr->SendClientsList(session_id);	//
		}
		else {
			serv_ptr->SendNewMessage(session_id, message);
		}
	}
}