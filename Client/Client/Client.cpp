// Client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <string>

#pragma warning(disable: 4996)

constexpr auto SIZE_MSG = 256;

SOCKET Connection;

void ClientHandler() {
	int msgSize;
	while (true) {
		recv(Connection, (char*)&msgSize, sizeof(int), NULL);
		char* msg = new char[msgSize + 1];
		msg[msgSize] = '\0';
		recv(Connection, msg, msgSize, NULL);
		std::cout << msg << std::endl;
		delete[] msg;
	}
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

	// сокет, для соединения с сервером
	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	// попытка соединения с сервером
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		std::cout << "Error: failed connect to server.\n";
		return 1;
	}
	std::cout << "Connected.\n";

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	std::string receivedMsg;
	while (true) {
		std::getline(std::cin, receivedMsg);
		int receiveMsgSize = receivedMsg.size();
		send(Connection, (char*)&receiveMsgSize, sizeof(int), NULL);
		send(Connection, receivedMsg.c_str(), receiveMsgSize, NULL);
		Sleep(10);
	}

	system("pause");
	return 0;
}