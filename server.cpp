#include<iostream>
#include<iomanip>
#include<WinSock2.h>
#include<WS2tcpip.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"Ws2_32.lib")

using namespace std;




int main() {
	//初期化
	WSADATA wsadata;
	if (WSAStartup(WINSOCK_VERSION, &wsadata) != 0)
	{
		cout << "初期化に失敗しました:" << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}



	//立てるサーバの検索
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	//検索条件の設定
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	char hostname[48];
	cout << "アドレスを入力してください＞";
	cin >> setw(32) >> hostname;

	//ポートに関するアドレス情報を取得
	int iResult = getaddrinfo(hostname, "50000", &hints, &result);
	if (iResult!=0)
	{
		cout << "情報の取得に失敗しました:" << iResult << endl;
		WSACleanup();
		return -1;
	}

	//取得した情報からソケットを作成
	SOCKET servsock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//この時点ではソケットにアドレスとポートはバインドされていない
	if (servsock==INVALID_SOCKET)
	{
		cout << "ソケット作成失敗:" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return -1;
	}

	//ソケットにアドレスとポート番号をバインドする
	iResult = bind(servsock, result->ai_addr, (int)result->ai_addrlen);
	if (iResult==SOCKET_ERROR)
	{
		cout << "bindに失敗しました:" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return -1;
	}

	freeaddrinfo(result);

	//コネクション待ちをするlisten
	if (listen(servsock,SOMAXCONN)==SOCKET_ERROR)
	{
		cout << "listenに失敗しました:" << WSAGetLastError() << endl;
		closesocket(servsock);
		WSACleanup();
		return -1;
	}


	//接続の受け入れaccept
	SOCKET clientsocket;
	clientsocket = accept(servsock, NULL, NULL);
	if (clientsocket==INVALID_SOCKET)
	{
		cout << "acceptに失敗しました:" << WSAGetLastError() << endl;
		closesocket(servsock);
		WSACleanup();
		return -1;

	}
	else
	{
		cout << "accept成功" << endl;
	}


	//データ送信、受信
	char recvbuf[512];
	int iSendResult;
	int recvbuflen = 512;
	
	do
	{
		iResult = recv(clientsocket, recvbuf, recvbuflen, 0);
		if (iResult>0)
		{
			//メッセージの受信
			cout << iResult << "Byte受取りました" << endl;
			cout << ">" << recvbuf << endl;

			//受け取った分送り返す
			iSendResult = send(clientsocket, recvbuf, iResult, 0);
			if (iSendResult==SOCKET_ERROR)
			{
				cout << "sendに失敗:" << WSAGetLastError();
				closesocket(clientsocket);
				WSACleanup();
				return -1;
			}
			//メッセージの送信
			cout << iSendResult << "Byte送りました" << endl;
			cout << "<" << recvbuf << endl;
		}
		else if (iResult==0)
		{
			//正常に終了
			cout << "connectionを閉じます" << endl;
		}
		else {
			cout << "recvに失敗:" << WSAGetLastError()<< endl;
			closesocket(clientsocket);
			WSACleanup();
			return -1;
		}

	} while (iResult>0);

	iResult = shutdown(clientsocket, SD_SEND);
	if (iResult==SOCKET_ERROR)
	{
		cout << "shutdownに失敗:" << WSAGetLastError() << endl;
		closesocket(clientsocket);
		WSACleanup();
		return -1;
	}

	closesocket(clientsocket);
	WSACleanup();
	

}