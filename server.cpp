#include<iostream>
#include<iomanip>
#include<WinSock2.h>
#include<WS2tcpip.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"Ws2_32.lib")

using namespace std;




int main() {
	//������
	WSADATA wsadata;
	if (WSAStartup(WINSOCK_VERSION, &wsadata) != 0)
	{
		cout << "�������Ɏ��s���܂���:" << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}



	//���Ă�T�[�o�̌���
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	//���������̐ݒ�
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	char hostname[48];
	cout << "�A�h���X����͂��Ă���������";
	cin >> setw(32) >> hostname;

	//�|�[�g�Ɋւ���A�h���X�����擾
	int iResult = getaddrinfo(hostname, "50000", &hints, &result);
	if (iResult!=0)
	{
		cout << "���̎擾�Ɏ��s���܂���:" << iResult << endl;
		WSACleanup();
		return -1;
	}

	//�擾������񂩂�\�P�b�g���쐬
	SOCKET servsock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//���̎��_�ł̓\�P�b�g�ɃA�h���X�ƃ|�[�g�̓o�C���h����Ă��Ȃ�
	if (servsock==INVALID_SOCKET)
	{
		cout << "�\�P�b�g�쐬���s:" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return -1;
	}

	//�\�P�b�g�ɃA�h���X�ƃ|�[�g�ԍ����o�C���h����
	iResult = bind(servsock, result->ai_addr, (int)result->ai_addrlen);
	if (iResult==SOCKET_ERROR)
	{
		cout << "bind�Ɏ��s���܂���:" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return -1;
	}

	freeaddrinfo(result);

	//�R�l�N�V�����҂�������listen
	if (listen(servsock,SOMAXCONN)==SOCKET_ERROR)
	{
		cout << "listen�Ɏ��s���܂���:" << WSAGetLastError() << endl;
		closesocket(servsock);
		WSACleanup();
		return -1;
	}


	//�ڑ��̎󂯓���accept
	SOCKET clientsocket;
	clientsocket = accept(servsock, NULL, NULL);
	if (clientsocket==INVALID_SOCKET)
	{
		cout << "accept�Ɏ��s���܂���:" << WSAGetLastError() << endl;
		closesocket(servsock);
		WSACleanup();
		return -1;

	}
	else
	{
		cout << "accept����" << endl;
	}


	//�f�[�^���M�A��M
	char recvbuf[512];
	int iSendResult;
	int recvbuflen = 512;
	
	do
	{
		iResult = recv(clientsocket, recvbuf, recvbuflen, 0);
		if (iResult>0)
		{
			//���b�Z�[�W�̎�M
			cout << iResult << "Byte����܂���" << endl;
			cout << ">" << recvbuf << endl;

			//�󂯎����������Ԃ�
			iSendResult = send(clientsocket, recvbuf, iResult, 0);
			if (iSendResult==SOCKET_ERROR)
			{
				cout << "send�Ɏ��s:" << WSAGetLastError();
				closesocket(clientsocket);
				WSACleanup();
				return -1;
			}
			//���b�Z�[�W�̑��M
			cout << iSendResult << "Byte����܂���" << endl;
			cout << "<" << recvbuf << endl;
		}
		else if (iResult==0)
		{
			//����ɏI��
			cout << "connection����܂�" << endl;
		}
		else {
			cout << "recv�Ɏ��s:" << WSAGetLastError()<< endl;
			closesocket(clientsocket);
			WSACleanup();
			return -1;
		}

	} while (iResult>0);

	iResult = shutdown(clientsocket, SD_SEND);
	if (iResult==SOCKET_ERROR)
	{
		cout << "shutdown�Ɏ��s:" << WSAGetLastError() << endl;
		closesocket(clientsocket);
		WSACleanup();
		return -1;
	}

	closesocket(clientsocket);
	WSACleanup();
	

}