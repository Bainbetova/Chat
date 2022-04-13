// Server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

#pragma warning(disable: 4996)
constexpr auto MAX_NUMBER_OF_CONNECTIONS = 100;
constexpr auto SIZE_MSG = 256;

SOCKET Connections[MAX_NUMBER_OF_CONNECTIONS]; // соединения
int Counter = 0; // индекс соединения

// тип пакета
enum Packet {
	PChatMessage,
	PTest
};

bool ProcessPacket(int index, Packet packettype) {
	int msgSize = 0;
	switch (packettype) {
	case PChatMessage:
	{
		// прием размера строки
		recv(Connections[index], (char*)&msgSize, sizeof(int), NULL);
		char* msg = new char[msgSize + 1];
		msg[msgSize] = '\0';
		// прием сообщения клиентов
		recv(Connections[index], msg, msgSize, NULL);
		// отправляем принятое сообщение всем соеднинениям, кроме того, который послал это сообщение
		for (int i = 0; i < Counter; i++) {
			if (i == index) {
				continue;
			}
			Packet msgtype = PChatMessage;
			send(Connections[i], (char*)&msgtype, sizeof(Packet), NULL);
			send(Connections[i], (char*)&msgSize, sizeof(int), NULL);
			send(Connections[i], msg, msgSize, NULL);
		}
		delete[] msg;
		break;
	}
	default:
		std::cout << "Unrecording packet: " << packettype << std::endl;
		break;
	}
	return true;
}

void ClientHandler(int index) {	
	Packet packettype;
	while (true) {
		recv(Connections[index], (char*)&packettype, sizeof(Packet), NULL);
		if (!ProcessPacket(index, packettype)) {
			break;
		}
	}
	closesocket(Connections[index]);
}

int main(int argc, char* argv[]) {
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	// привязка ареса к сокету
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	// прослушиваем порт
	listen(sListen, SOMAXCONN);

	// сокет, для соединения с клиентом
	SOCKET newConnection;
	for (int i = 0; i < MAX_NUMBER_OF_CONNECTIONS; i++) {
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else {
			std::cout << "Client Connected.\n";
			std::string msg = "Hello.";
			int msgSize = msg.size();
			Packet msgtype = PChatMessage;
			send(newConnection, (char*)&msgtype, sizeof(Packet), NULL);
			send(newConnection, (char*)&msgSize, sizeof(int), NULL);
			send(newConnection, msg.c_str(), msgSize, NULL);

			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)i, NULL, NULL);
		
			Packet testpacket = PTest;
			send(newConnection, (char*)&testpacket, sizeof(Packet), NULL);
		}
	}

	system("pause");



	return 0;
}