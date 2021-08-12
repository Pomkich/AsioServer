#include <iostream>
#include <string>
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#include <asio.hpp>
#include <functional>
#include <memory>
#include <istream>

using namespace std;
using namespace asio;

class client_session : public enable_shared_from_this<client_session> {
private:
	error_code err;
	// Server* serv_prt;
	int session_id;
	ip::tcp::socket sock;
	enum { max_buf = 1024 };
	char read_buf[max_buf];
	char write_buf[max_buf];

public:
	static shared_ptr<client_session> Create(io_context& con) {
		shared_ptr<client_session> new_client_ses(new client_session(con));
		return new_client_ses;
	}

	void Start() {
		cout << "client session started, waiting for message..." << endl;
		Write("you're welcome!\n");
		sock.async_receive(buffer(read_buf), std::bind(&client_session::OnRead, shared_from_this()));
	}

	void Write(string mes) {
		std::copy(mes.begin(), mes.end(), write_buf);
		sock.async_send(buffer(write_buf, mes.size()), std::bind(&client_session::OnWrite, shared_from_this()));
	}

	ip::tcp::socket& GetSocket() { return sock; }
	int GetSessionId() { return session_id; }

private:
	client_session(io_context& con) : sock(con) {};

	void Read() {
		sock.async_receive(buffer(read_buf), std::bind(&client_session::OnRead, shared_from_this()));
	}

	void OnWrite() {
		cout << "message was send" << endl;
	}

	void OnRead() {
		cout << "message received: " << read_buf << endl;
		HandleMessage();
		memset(read_buf, 0x00, 1024);
		Read();
	}

	void HandleMessage() {
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
			// Server.SendClientsList(session_id);	//
		}
		else if (message == "-disconect") {
			// Server.CloseSession(session_id);
		}
		else {
			// Server.SendNewMessage(session_id);
		}
	}
};

class Server {
private:
	io_context context;
	ip::tcp::acceptor acceptor;
	vector<shared_ptr<client_session>> clients;

public:
	Server(ip::tcp::endpoint ep) : acceptor(context, ep) {};

	void Run() {
		error_code err;
		shared_ptr<client_session> new_client_ses = client_session::Create(context);
		acceptor.async_accept(new_client_ses->GetSocket(), std::bind(&Server::AcceptHandler, this, new_client_ses, err));
		context.run();
	}

private:

	void AcceptHandler(std::shared_ptr<client_session> clt, error_code& err) {
		cout << "new connection " << endl;
		clients.push_back(clt);
		clt->Start();
		shared_ptr<client_session> new_client_ses = client_session::Create(context);
		acceptor.async_accept(new_client_ses->GetSocket(), std::bind(&Server::AcceptHandler, this, new_client_ses, err));
	}

};

int main(int argc, char* argv[])
{
	error_code err;
	Server serv(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8001));

	serv.Run();
}