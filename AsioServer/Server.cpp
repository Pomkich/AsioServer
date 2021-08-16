#include "Server.h"

Server::Server(ip::tcp::endpoint ep) : acceptor(context, ep) {};

void Server::Run() {
	error_code err;
	shared_ptr<client_session> new_client_ses = client_session::Create(context, this, id_iter);
	acceptor.async_accept(new_client_ses->GetSocket(), std::bind(&Server::AcceptHandler, this, new_client_ses, err));
	new_client_ses.reset();
	context.run();
}

void Server::SendClientsList(int session_id) {
	string clt_list = "\nClients on this server:\n";
	for (int i = 0; i < clients.size(); i++) {
		string clt = "\t";
		clt += to_string(clients[i]->GetSessionId());
		clt += ";\n";
		clt_list += clt;
	}
	clients[session_id]->Write(clt_list);
}

void Server::CloseSession(int session_id) {
	auto del_clt = find_if(clients.begin(), clients.end(), [&](shared_ptr<client_session> clt) {return (clt->GetSessionId() == session_id); });
	del_clt->get()->Stop();
	clients.erase(del_clt);
}

void Server::SendNewMessage(int session_id, string message) {
	for (int i = 0; i < clients.size(); i++) {
		if (i != session_id) {
			clients[i]->Write(message);
		}
	}
}

void Server::AcceptHandler(std::shared_ptr<client_session> clt, error_code& err) {
	cout << "new connection " << endl;
	clients.push_back(clt);
	clt->Start();
	id_iter++;
	shared_ptr<client_session> new_client_ses = client_session::Create(context, this, id_iter);
	acceptor.async_accept(new_client_ses->GetSocket(), std::bind(&Server::AcceptHandler, this, new_client_ses, err));
}
