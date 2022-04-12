// Server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

#pragma warning(disable: 4996)
constexpr auto MAX_NUMBER_OF_CONNECTIONS = 100;
constexpr auto SIZE_MSG = 256;

SOCKET Connections[MAX_NUMBER_OF_CONNECTIONS]; // соединения
int Counter = 0; // индекс соединения

void ClientHandler(int index) {
	char msg[SIZE_MSG];
	while (true) {
		// прием сообщения клиентов
		recv(Connections[index], msg, sizeof(msg), NULL);
		// отправляем принятое сообщение всем соеднинениям, кроме того, который послал это сообщение
		for (int i = 0; i < Counter; i++) {
			if (i == index) {
				continue;
			}
			send(Connections[i], msg, sizeof(msg), NULL);
		}
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
			char msg[SIZE_MSG] = "Hello.";
			send(newConnection, msg, sizeof(msg), NULL);

			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)i, NULL, NULL);
		}
	}

	system("pause");



	return 0;
}