#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Queue.h"

#define SERVERPORT 9000
#define BUFSIZE    512
#define MAXUSER 10
#define NO_DATA -1
#define RADIUS 20

struct PACKET;
struct OVERLAPPEDEX;
struct SOCKETINFO;
struct Mission;

enum IOTYPE
{
	IO_READ, // �б���� 
	IO_WRITE // ������� 
};

enum PROTOCAL
{
	NONE_USER,		// ������ �ƹ��۾��� ���Ҷ� 
	CREATE_USER,	// ���� ������ �������� 
	SETTING_USER,	// ���ε��� ������ ������ �������ְ� �����ٶ� 
	MOVE_USER,		// ������ ���������� 
	UPDATE_USER,	// �ٲ������� �˷��ٶ� 
	DELETE_USER		// ������ �������� 
};

enum TRANSSTATE
{
	DISCONNECT = -1,		// ������ ������ �� 
	SOC_FALSE,				// �ۼ����� �� ������ 
	SOC_TRUE				// �ۼ����� �Ϸ������ 
};

// �۾��� ������ �Լ�
DWORD WINAPI WorkerThread(LPVOID arg);
// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);
//���� �߰��Լ�
SOCKETINFO* AddSocketInfo(SOCKET _sock);
//���� ���� �Լ�
bool RemoveSocketInfo(SOCKETINFO* _ptr);
//iocp �б����
void IO_Read(SOCKETINFO* _info, DWORD _cbTransferred);
//iocp �������
void IO_Write(SOCKETINFO* _info, DWORD _cbTransferred);
//iocp�� wsarecv�Լ�
int IOCP_Recv(SOCKETINFO* _info);
//iocp�� wsasend�Լ�
int IOCP_Send(SOCKETINFO* _info);
//��ŷ�� �Լ�
BOOL Packing(int _totalsize, char* _buf, PROTOCAL _protocal, int _size, char* _data);
//����ŷ�� �Լ�
BOOL UnPacking(SOCKETINFO* _info, PROTOCAL* _protocal);
//�� �޾Ҵ��� Ȯ���ϴ� �Լ�
BOOL CheckRecv(SOCKETINFO* _info);
//�� ���´��� Ȯ���ϴ� �Լ�
BOOL CheckSend(SOCKETINFO* _info, DWORD _cbTransferred);
//ť�� �־��ִ� �Լ�
int PushQueue(SOCKETINFO* _info, Mission* _mission);


struct Mission	// �̼��� ����ü 
{
	char* buf;	// �������� ������ �޸𸮺���Ǿ� �尡�� ���� 
	int size;		// �ѱ��� 

	// ��������,���ڱ���,���ڳ��� 
	Mission(PROTOCAL _protocal = PROTOCAL::NONE_USER, int _size = 0, char* _data = NULL)
	{
		size =					// �ѱ��� 
			sizeof(int)+		// �ѱ���ũ�� 
			sizeof(PROTOCAL)+	// ��������ũ�� 
			sizeof(int)+		// ���ڱ���ũ�� 
			_size;				// ���ڱ��� 
		buf = new char[size];
		Packing(size, buf, _protocal, _size, _data);
	}
	
};

struct CIRCLE		// �� ���� ����ü 
{
	int idnum;		// ����������ȣ 
	POINT pos;		// ������ǥ 
	int radius;		// ���� ������ 
	COLORREF color;	// ���� RGB�� 
	bool login;		// ������ �������� ���� 
};

struct PACKET		// ��Ŷ ����ü 
{
	char SendBuf[BUFSIZE];	// ������ ���� ���� 
	char RecvBuf[BUFSIZE];  // ������ ���� ���� 

	int Comp_Recvbytes;		// ��ŭ �޾Ҵ��� ���� 
	int Recvbytes;			// �� �޾ƾ��� ���� 
	int Comp_Sendbytes;		// ��ŭ ���´��� ���� 
	int Sendbytes;			// �� �������� ���� 

	int len;				// ���� ���� 
};

struct OVERLAPPEDEX			// ������ex����ü 
{
	OVERLAPPED overlapped;	// ����������ü 
	SOCKETINFO* info;		// �ڱ��ڽ� 
	IOTYPE iotype;			// �б�,������� 
};

// ���� ���� ������ ���� ����ü
struct SOCKETINFO
{
	OVERLAPPEDEX ReadOverlapped;		// �б����������ü 
	OVERLAPPEDEX WriteOverlapped;		// �������������ü 
	SOCKET sock;						// ���� 
	PACKET* packet;						// ��Ŷ 
	CIRCLE circle;						// �� 
	Queue<Mission*> queue;				// ť 
};


int UserCount = 0; // �� ���� ���� 
SOCKETINFO* User[MAXUSER]; // �����迭 

DWORD recvbytes, sendbytes;
DWORD flags = 0;
CRITICAL_SECTION cs;

int main(int argc, char *argv[])
{
	int retval;
	InitializeCriticalSection(&cs);
	srand(time(NULL));
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// ����� �Ϸ� ��Ʈ ����
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) return 1;

	// CPU ���� Ȯ��
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// (CPU ���� * 2)���� �۾��� ������ ����
	HANDLE hThread;
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; i++)
	{
		hThread = CreateThread(NULL, 0, WorkerThread, hcp, 0, NULL);
		if (hThread == NULL)
			return 1;
		CloseHandle(hThread);
	}

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int result;
	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		SOCKETINFO* ptr = AddSocketInfo(client_sock);

		// ���ϰ� ����� �Ϸ� ��Ʈ ����
		CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);

		//���� ���� �������� WSARecv�� �����ش�
		result = IOCP_Recv(ptr);
		if (result == TRANSSTATE::DISCONNECT)
		{
			RemoveSocketInfo(ptr);
		}

		EnterCriticalSection(&cs);
		for (int i = 0; i < UserCount; i++)
		{
			if (User[i]->circle.login == true)
			{
				if (PushQueue(ptr, new Mission(PROTOCAL::UPDATE_USER, sizeof(CIRCLE), (char*)&User[i]->circle)) == TRANSSTATE::DISCONNECT)
				{
					RemoveSocketInfo(ptr);
				}
			}
		}
		LeaveCriticalSection(&cs);

	}
	DeleteCriticalSection(&cs);
	// ���� ����
	WSACleanup();
	return 0;
}

// �۾��� ������ �Լ�
DWORD WINAPI WorkerThread(LPVOID arg)
{
	int retval;
	HANDLE hcp = (HANDLE)arg;

	while (1)
	{
		// �񵿱� ����� �Ϸ� ��ٸ���
		DWORD cbTransferred;
		SOCKET client_sock;
		OVERLAPPEDEX *ptr = NULL;
		SOCKETINFO* info = NULL;
		retval = GetQueuedCompletionStatus(hcp, &cbTransferred, (LPDWORD)&client_sock, (LPOVERLAPPED *)&ptr, INFINITE);
		info = ptr->info;

		// Ŭ���̾�Ʈ ���� ���
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(info->sock, (SOCKADDR *)&clientaddr, &addrlen);
		// �񵿱� ����� ��� Ȯ��
		if (retval == 0 || cbTransferred == 0)
		{
			if (retval == 0)
			{
				DWORD temp1, temp2;
				WSAGetOverlappedResult(info->sock, (LPWSAOVERLAPPED)&info->ReadOverlapped, &temp1, FALSE, &temp2);
				err_display("WSAGetOverlappedResult()");
				RemoveSocketInfo(info);
				continue;
			}
			RemoveSocketInfo(info);
		}
		switch (ptr->iotype)
		{
		case IO_READ:
			printf("IO_READ\n");
			IO_Read(ptr->info, cbTransferred);
			break;
		case IO_WRITE:
			printf("IO_WRITE\n");
			IO_Write(ptr->info, cbTransferred);
			break;
		}
	}
	return 0;
}

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

SOCKETINFO* AddSocketInfo(SOCKET _sock)
{
	int retval;

	// ���� ���� ����ü �Ҵ�
	EnterCriticalSection(&cs);
	if (UserCount >= MAXUSER)
	{
		LeaveCriticalSection(&cs);
		return FALSE;
	}
	SOCKETINFO *ptr = new SOCKETINFO;
	if (ptr == NULL)
	{
		LeaveCriticalSection(&cs);
		return FALSE;
	}
	ptr->packet = new PACKET;
	ZeroMemory(ptr->packet, sizeof(PACKET));
	ZeroMemory(&ptr->ReadOverlapped, sizeof(ptr->ReadOverlapped));
	ZeroMemory(&ptr->WriteOverlapped, sizeof(ptr->WriteOverlapped));
	ptr->ReadOverlapped.iotype = IO_READ;
	ptr->WriteOverlapped.iotype = IO_WRITE;
	ptr->ReadOverlapped.info = ptr;
	ptr->WriteOverlapped.info = ptr;

	//�������� �ʱ�ȭ
	ptr->sock = _sock;

	ptr->packet->Recvbytes = ptr->packet->Sendbytes = 0;
	ptr->packet->Comp_Recvbytes = ptr->packet->Comp_Sendbytes = 0;
	ptr->packet->len = 0;

	ptr->circle.idnum = UserCount;											// �������̵� ���� ����ī��Ʈ�� �־��ش� 
	ptr->circle.pos.x = 0;													// ���� ó�� x��ǥ �ʱ�ȭ 
	ptr->circle.pos.y = 0;													// ���� ó�� y��ǥ �ʱ�ȭ 
	ptr->circle.radius = RADIUS;											// ���� ������ �ʱ�ȭ 
	ptr->circle.color = RGB(rand() % 256, rand() % 256, rand() % 256);		// ���� �� ���� �ʱ�ȭ 
	ptr->circle.login = false;												// ���� �α��κҰ� �ʱ�ȭ 

	User[UserCount++] = ptr;												// ��������ü�� ���� ���� ���� �־��ֱ� 
	LeaveCriticalSection(&cs);
	return ptr;
}

bool RemoveSocketInfo(SOCKETINFO* _ptr)
{
	EnterCriticalSection(&cs);

	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);

	for (int i = 0; i < UserCount; i++)
	{
		if (User[i]->sock == _ptr->sock)
		{
			getpeername(_ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);
			_ptr->circle.login = false;
			//������ �����ٰ� Ŭ���̾�Ʈ���� �˷��ֱ�
			for (int i = 0; i < UserCount; i++)
			{
				PushQueue(User[i], new Mission(PROTOCAL::DELETE_USER, sizeof(CIRCLE), (char*)&_ptr->circle));
			}

			closesocket(_ptr->sock);
			delete _ptr;
			_ptr = nullptr;

			for (int j = i + 1; j < UserCount; ++j, ++i)
			{
				User[i] = User[j];
			}
			UserCount--;
			User[UserCount] = nullptr;

			printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));



			LeaveCriticalSection(&cs);
			return true;
		}
	}
	LeaveCriticalSection(&cs);
	return false;
}

void IO_Read(SOCKETINFO* _info, DWORD _cbTransferred)
{
	int retval;
	PROTOCAL protocal = PROTOCAL::NONE_USER;
	//�� ����Ŭ �������� ���� _cbTransferred���� ������ ������� ���� ���̿� �־��ش�
	_info->packet->Comp_Recvbytes += _cbTransferred;
	//��� ���ú��۾��� �������� üũ
	if (CheckRecv(_info) == true)
	{
		//����ŷ�۾�
		UnPacking(_info, &protocal);

		//���¿� ���� ���� �����ֱ�
		switch (protocal)
		{
		case PROTOCAL::CREATE_USER:
			EnterCriticalSection(&cs);
			memcpy(&_info->circle.pos, _info->packet->RecvBuf, sizeof(POINT));
			_info->circle.login = true;
			PushQueue(_info, new Mission(PROTOCAL::SETTING_USER, sizeof(CIRCLE), (char*)&_info->circle));
			for (int i = 0; i < UserCount; i++)
			{
				if (PushQueue(User[i], new Mission(PROTOCAL::UPDATE_USER, sizeof(CIRCLE), (char*)&_info->circle)) == TRANSSTATE::DISCONNECT)
				{
					RemoveSocketInfo(_info);
				}
			}
			break;
		case PROTOCAL::MOVE_USER:
			EnterCriticalSection(&cs);
			memcpy(&_info->circle.pos, _info->packet->RecvBuf, sizeof(POINT));
			for (int i = 0; i < UserCount; i++)			// ������ ��ŭ ������.
			{
				if (PushQueue(User[i], new Mission(PROTOCAL::UPDATE_USER, sizeof(CIRCLE), (char*)&_info->circle)) == TRANSSTATE::DISCONNECT)
				{
					RemoveSocketInfo(_info);
				}
			}
			break;
		}
		//�ٽ� ���ú� �����ֱ�
		retval = IOCP_Recv(_info);
		if (retval == TRANSSTATE::DISCONNECT)
		{
			RemoveSocketInfo(_info);
		}
		LeaveCriticalSection(&cs);
	}
	//�ȳ������� �ٽ� ���ú곯���ֱ�
	else
	{
		retval = IOCP_Recv(_info);
		if (retval == TRANSSTATE::DISCONNECT)
		{
			RemoveSocketInfo(_info);
		}
	}
}


void IO_Write(SOCKETINFO* _info, DWORD _cbTransferred)
{
	int retval;

	if (CheckSend(_info, _cbTransferred) == true)
	{
		//�������̿� �������� 0���� �ʱ�ȭ
		_info->packet->Comp_Sendbytes = _info->packet->Sendbytes = 0;
		//�� �������� ������ ť�� �ִ� ���� ��
		_info->queue.Pop();
		//������ ť�� ���� ������ ���� ���
		if (_info->queue.QIsEmpty() != true)
		{
			retval = IOCP_Send(_info);
			if (retval == TRANSSTATE::DISCONNECT)
			{
				RemoveSocketInfo(_info);
			}
		}
	}
	else
	{
		//WSASend �ٽ� ȣ��
		retval = IOCP_Send(_info);
		if (retval == TRANSSTATE::DISCONNECT)
		{
			RemoveSocketInfo(_info);
		}
	}
}

//iocp�� wsarecv�Լ�
int IOCP_Recv(SOCKETINFO* _info)
{
	int retval;
	WSABUF wsabuf;
	ZeroMemory(&_info->ReadOverlapped.overlapped, sizeof(_info->ReadOverlapped.overlapped));
	ZeroMemory(&wsabuf, sizeof(wsabuf));

	wsabuf.buf = _info->packet->RecvBuf + _info->packet->Comp_Recvbytes;
	wsabuf.len = BUFSIZE;
	//printf("RecvBytes : %d\n", _info->packet->Recvbytes);

	retval = WSARecv(_info->sock, &wsabuf, 1, &recvbytes, &flags, &_info->ReadOverlapped.overlapped, NULL);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			err_display("WSARecv()");
			return TRANSSTATE::DISCONNECT;
		}
		return TRANSSTATE::SOC_FALSE;
	}
	return TRANSSTATE::SOC_TRUE;
}

//iocp�� wsasend�Լ�
int IOCP_Send(SOCKETINFO* _info)
{
	WSABUF wsabuf;
	ZeroMemory(&wsabuf, sizeof(wsabuf));
	//���� ť�� �� �տ� �ִ� �̼��� mission�� �ֱ�
	Mission* mission = _info->queue.Front();
	//������ ���� �� ���� = �̼��� size
	_info->packet->Sendbytes = mission->size;
	//������ ����������� ����������ü �ʱ�ȭ
	ZeroMemory(&_info->WriteOverlapped.overlapped, sizeof(_info->WriteOverlapped.overlapped));

	wsabuf.buf = mission->buf + _info->packet->Comp_Sendbytes;
	wsabuf.len = _info->packet->Sendbytes - _info->packet->Comp_Sendbytes;

	int retval = WSASend(_info->sock, &wsabuf, 1, &sendbytes, 0, &_info->WriteOverlapped.overlapped, NULL);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			err_display("WSASend()");
			return TRANSSTATE::DISCONNECT;
		}
		return TRANSSTATE::SOC_FALSE;
	}
	return TRANSSTATE::SOC_TRUE;
}

BOOL Packing(int _totalsize, char* _buf, PROTOCAL _protocal, int _size, char* _data)
{
	ZeroMemory(_buf, sizeof(_buf));
	memcpy
		(
		_buf,// buf 
		(char*)&_totalsize,// ��Ż������ 
		sizeof(int)// ��Ż������ ũ�� 
		);

	memcpy
		(
		_buf + sizeof(int),// buf+sizeof(int) 
		(char*)&_protocal,// �������� �� 
		sizeof(PROTOCAL)// �������� ũ�� 
		);

	memcpy
		(
		_buf + sizeof(int)+sizeof(PROTOCAL),// buf+sizeof(int)+sizeof(PROTOCAL) 
		(char*)&_size,// �������ڱ��� 
		sizeof(int)// ����ũ�� 
		);

	memcpy
		(
		_buf + sizeof(int)+sizeof(PROTOCAL)+sizeof(int),// buf+sizeof(int)+sizeof(PROTOCAL)+sizeof(int) 
		_data,// ���� ���ڳ��� 
		_size// ���ڱ��� 
		);
	return true;
}

BOOL UnPacking(SOCKETINFO* _info, PROTOCAL* _protocal)
{
	memcpy
		(
		_protocal,// �������� 
		_info->packet->RecvBuf + sizeof(int),// _info->packet->RecvBuf + sizeof(int) 
		sizeof(PROTOCAL)// �������� ũ�� 
		);

	memcpy
		(
		&_info->packet->len,// _info->circle.pos�� ���� 
		_info->packet->RecvBuf + sizeof(int)+sizeof(PROTOCAL),// _info->packet->RecvBuf + sizeof(int) + sizeof(PROTOCAL) 
		sizeof(int)// ��Ʈ�� ũ�� 
		);

	memcpy
		(
		_info->packet->RecvBuf,// ���� 
		_info->packet->RecvBuf + sizeof(int)+sizeof(PROTOCAL)+sizeof(int),// _info->packet->RecvBuf + sizeof(int) + sizeof(PROTOCAL) + sizeof(int) 
		_info->packet->len// ���� 
		);
	//printf("[recv] x : %d, y : %d\n", _info->circle.pos.x, _info->circle.pos.y);
	return true;
}

int PushQueue(SOCKETINFO* _info, Mission* _mission)
{
	//������ ť�� �̼��� �ֱ����� ť�� ������� Ȯ��
	if (_info->queue.QIsEmpty() == true)
	{
		//������ ť�� �̼��� �־��ش�.
		_info->queue.Push(_mission);
		//ť�� ���빰�� �Ѱ��̹Ƿ� �ٷ� send�� �����ش�
		return IOCP_Send(_info);
	}
	//ť�� ������� �ʰ� ���빰�� ������ �׳� �־��ֱ⸸ �Ѵ�
	else
	{
		_info->queue.Push(_mission);
	}
	return TRANSSTATE::SOC_TRUE;
}

BOOL CheckRecv(SOCKETINFO* _info)
{
	//��ó�� �� ���� ���̰� 0�̰� ������� ���� ���̰� sizeof(int)���� ũ�Ű� ���� ���
	if (_info->packet->Recvbytes == 0)
	{
		//�� �޾��� ���
		if (_info->packet->Comp_Recvbytes >= sizeof(int))
		{
			//�տ� ��Ż����� _info->packet->Recvbytes�� �־��ش�
			memcpy(&_info->packet->Recvbytes, _info->packet->RecvBuf, sizeof(int));
		}
	}
	//�� ���� ���̰� 0�� �ƴ� ���(��ó�� 4����Ʈ�� ��Ż����� �޾Ƽ� �� ���� ����Ǿ� ���� ���

	//���� ���� ���̿� ���� ���̰� ���� ���(���ú갡 ���� ���)
	if (_info->packet->Comp_Recvbytes >= _info->packet->Recvbytes && _info->packet->Recvbytes != 0)
	{
		if (_info->packet->Comp_Recvbytes == _info->packet->Recvbytes)
		{
			_info->packet->Comp_Recvbytes = _info->packet->Recvbytes = 0;
			return true;
		}
		else if (_info->packet->Comp_Recvbytes > _info->packet->Recvbytes)
		{
			memmove(_info->packet->RecvBuf, _info->packet->RecvBuf + _info->packet->Recvbytes, _info->packet->Comp_Recvbytes - _info->packet->Recvbytes);
			_info->packet->Comp_Recvbytes = _info->packet->Comp_Recvbytes - _info->packet->Recvbytes;
			_info->packet->Recvbytes = 0;
		}
	}
	return false;
}

BOOL CheckSend(SOCKETINFO* _info, DWORD _cbTransferred)
{
	_info->packet->Comp_Sendbytes += _cbTransferred;
	//�� ������ ���
	if (_info->packet->Comp_Sendbytes == _info->packet->Sendbytes)
	{
		_info->packet->Comp_Sendbytes = _info->packet->Sendbytes = 0;
		return true;
	}
	else
	{
		return false;
	}
}