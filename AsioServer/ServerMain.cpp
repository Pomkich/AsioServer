#include <iostream>
#include <string>
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#include <asio.hpp>
#include <functional>
#include <memory>

using namespace std;
using namespace asio;

class client_session : public enable_shared_from_this<client_session> {
private:
	ip::tcp::socket sock;
	enum { max_buf = 1024 };
	char read_buf[max_buf];
	char write_buf[max_buf];

public:
	static shared_ptr<client_session> Create(io_context& con) {
		shared_ptr<client_session> new_client_ses(new client_session(con));
		return new_client_ses;
	}

	ip::tcp::socket& GetSocket() { return sock; }

private:
	client_session(io_context& con) : sock(con) {};
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
		//clt->Start()
		shared_ptr<client_session> new_client_ses = client_session::Create(context);
		clients.push_back(new_client_ses);
		acceptor.async_accept(new_client_ses->GetSocket(), std::bind(&Server::AcceptHandler, this, new_client_ses, err));
	}
};

int main(int argc, char* argv[])
{
	error_code err;
	Server serv(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8001));

	serv.Run();
}