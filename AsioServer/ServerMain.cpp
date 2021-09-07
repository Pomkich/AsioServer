#include <iostream>
#include <string>
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#include <asio.hpp>
#include <functional>
#include <memory>
#include <istream>
#include "Server.h"

using namespace std;
using namespace asio;


int main(int argc, char* argv[])
{
	error_code err;
	cout << "your address: " << ip::address_v4() << endl;
	Server serv(ip::tcp::endpoint(ip::address_v4(), 8001));

	serv.Run();
}