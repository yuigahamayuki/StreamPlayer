#include "NetModule.h"

#pragma comment(lib, "ws2_32.lib")

NetModule::NetModule(const char * fileName)
	: _fileName(fileName), _sockfd(0)
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);

	memset(&_serverSockAddr, 0, sizeof(sockaddr_in));
	_serverSockAddr.sin_family = AF_INET;
	_serverSockAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_serverSockAddr.sin_port = htons(6666);

	_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

NetModule::~NetModule()
{
	closesocket(_sockfd);
	WSACleanup();
}

void NetModule::init(ReadBuffer& readBuffer)
{

	int write_size = sendto(_sockfd, _fileName, strlen(_fileName), 0, (const sockaddr*)&_serverSockAddr, sizeof(_serverSockAddr));

	
	int read_size = recvfrom(_sockfd, readBuffer.buf(), readBuffer.sizeReadable(), 0, NULL, NULL);

	
}
