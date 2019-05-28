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
	IO_READ, // 읽기상태 
	IO_WRITE // 쓰기상태 
};

enum PROTOCAL
{
	NONE_USER,		// 유저가 아무작업도 안할때 
	CREATE_USER,	// 새로 유저가 들어왔을때 
	SETTING_USER,	// 새로들어온 유저의 정보를 세팅해주고 보내줄때 
	MOVE_USER,		// 유저가 움직였을때 
	UPDATE_USER,	// 바뀐정보를 알려줄때 
	DELETE_USER		// 유저가 나갔을때 
};

enum TRANSSTATE
{
	DISCONNECT = -1,		// 유저가 나갔을 때 
	SOC_FALSE,				// 송수신이 덜 됬을때 
	SOC_TRUE				// 송수신이 완료됬을때 
};

// 작업자 스레드 함수
DWORD WINAPI WorkerThread(LPVOID arg);
// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);
//유저 추가함수
SOCKETINFO* AddSocketInfo(SOCKET _sock);
//유저 삭제 함수
bool RemoveSocketInfo(SOCKETINFO* _ptr);
//iocp 읽기상태
void IO_Read(SOCKETINFO* _info, DWORD _cbTransferred);
//iocp 쓰기상태
void IO_Write(SOCKETINFO* _info, DWORD _cbTransferred);
//iocp용 wsarecv함수
int IOCP_Recv(SOCKETINFO* _info);
//iocp용 wsasend함수
int IOCP_Send(SOCKETINFO* _info);
//패킹용 함수
BOOL Packing(int _totalsize, char* _buf, PROTOCAL _protocal, int _size, char* _data);
//언패킹용 함수
BOOL UnPacking(SOCKETINFO* _info, PROTOCAL* _protocal);
//다 받았는지 확인하는 함수
BOOL CheckRecv(SOCKETINFO* _info);
//다 보냈는지 확인하는 함수
BOOL CheckSend(SOCKETINFO* _info, DWORD _cbTransferred);
//큐에 넣어주는 함수
int PushQueue(SOCKETINFO* _info, Mission* _mission);


struct Mission	// 미션지 구조체 
{
	char* buf;	// 가변길이 했을때 메모리복사되어 드가는 버퍼 
	int size;		// 총길이 

	// 프로토콜,문자길이,문자내용 
	Mission(PROTOCAL _protocal = PROTOCAL::NONE_USER, int _size = 0, char* _data = NULL)
	{
		size =					// 총길이 
			sizeof(int)+		// 총길이크기 
			sizeof(PROTOCAL)+	// 프로토콜크기 
			sizeof(int)+		// 문자길이크기 
			_size;				// 문자길이 
		buf = new char[size];
		Packing(size, buf, _protocal, _size, _data);
	}
	
};

struct CIRCLE		// 원 정보 구조체 
{
	int idnum;		// 유저고유번호 
	POINT pos;		// 유저좌표 
	int radius;		// 유저 반지름 
	COLORREF color;	// 유저 RGB색 
	bool login;		// 유저가 접속중인 상태 
};

struct PACKET		// 패킷 구조체 
{
	char SendBuf[BUFSIZE];	// 보낼때 쓰는 버퍼 
	char RecvBuf[BUFSIZE];  // 받을때 쓰는 버퍼 

	int Comp_Recvbytes;		// 얼만큼 받았는지 길이 
	int Recvbytes;			// 총 받아야할 길이 
	int Comp_Sendbytes;		// 얼만큼 보냈는지 길이 
	int Sendbytes;			// 총 보내야할 길이 

	int len;				// 문자 길이 
};

struct OVERLAPPEDEX			// 오버랩ex구조체 
{
	OVERLAPPED overlapped;	// 오버랩구조체 
	SOCKETINFO* info;		// 자기자신 
	IOTYPE iotype;			// 읽기,쓰기상태 
};

// 소켓 정보 저장을 위한 구조체
struct SOCKETINFO
{
	OVERLAPPEDEX ReadOverlapped;		// 읽기오버랩구조체 
	OVERLAPPEDEX WriteOverlapped;		// 쓰기오버랩구조체 
	SOCKET sock;						// 소켓 
	PACKET* packet;						// 패킷 
	CIRCLE circle;						// 원 
	Queue<Mission*> queue;				// 큐 
};


int UserCount = 0; // 총 유저 갯수 
SOCKETINFO* User[MAXUSER]; // 유저배열 

DWORD recvbytes, sendbytes;
DWORD flags = 0;
CRITICAL_SECTION cs;

int main(int argc, char *argv[])
{
	int retval;
	InitializeCriticalSection(&cs);
	srand(time(NULL));
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// 입출력 완료 포트 생성
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) return 1;

	// CPU 개수 확인
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// (CPU 개수 * 2)개의 작업자 스레드 생성
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

	// 데이터 통신에 사용할 변수
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
		printf("[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		SOCKETINFO* ptr = AddSocketInfo(client_sock);

		// 소켓과 입출력 완료 포트 연결
		CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);

		//새로 들어온 유저에게 WSARecv를 날려준다
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
	// 윈속 종료
	WSACleanup();
	return 0;
}

// 작업자 스레드 함수
DWORD WINAPI WorkerThread(LPVOID arg)
{
	int retval;
	HANDLE hcp = (HANDLE)arg;

	while (1)
	{
		// 비동기 입출력 완료 기다리기
		DWORD cbTransferred;
		SOCKET client_sock;
		OVERLAPPEDEX *ptr = NULL;
		SOCKETINFO* info = NULL;
		retval = GetQueuedCompletionStatus(hcp, &cbTransferred, (LPDWORD)&client_sock, (LPOVERLAPPED *)&ptr, INFINITE);
		info = ptr->info;

		// 클라이언트 정보 얻기
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(info->sock, (SOCKADDR *)&clientaddr, &addrlen);
		// 비동기 입출력 결과 확인
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

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

	// 소켓 정보 구조체 할당
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

	//유저정보 초기화
	ptr->sock = _sock;

	ptr->packet->Recvbytes = ptr->packet->Sendbytes = 0;
	ptr->packet->Comp_Recvbytes = ptr->packet->Comp_Sendbytes = 0;
	ptr->packet->len = 0;

	ptr->circle.idnum = UserCount;											// 유저아이디를 현재 유저카운트로 넣어준다 
	ptr->circle.pos.x = 0;													// 유저 처음 x좌표 초기화 
	ptr->circle.pos.y = 0;													// 유저 처음 y좌표 초기화 
	ptr->circle.radius = RADIUS;											// 유저 반지름 초기화 
	ptr->circle.color = RGB(rand() % 256, rand() % 256, rand() % 256);		// 유저 원 색깔 초기화 
	ptr->circle.login = false;												// 유저 로그인불값 초기화 

	User[UserCount++] = ptr;												// 유저구조체에 들어온 유저 정보 넣어주기 
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
			//유저가 나갔다고 클라이언트에게 알려주기
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

			printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));



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
	//한 싸이클 돌았을때 들어온 _cbTransferred값을 유저가 현재까지 받은 길이에 넣어준다
	_info->packet->Comp_Recvbytes += _cbTransferred;
	//모든 리시브작업이 끝났는지 체크
	if (CheckRecv(_info) == true)
	{
		//언패킹작업
		UnPacking(_info, &protocal);

		//상태에 따라 샌드 날려주기
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
			for (int i = 0; i < UserCount; i++)			// 유저수 만큼 보내줌.
			{
				if (PushQueue(User[i], new Mission(PROTOCAL::UPDATE_USER, sizeof(CIRCLE), (char*)&_info->circle)) == TRANSSTATE::DISCONNECT)
				{
					RemoveSocketInfo(_info);
				}
			}
			break;
		}
		//다시 리시브 날려주기
		retval = IOCP_Recv(_info);
		if (retval == TRANSSTATE::DISCONNECT)
		{
			RemoveSocketInfo(_info);
		}
		LeaveCriticalSection(&cs);
	}
	//안끝났으면 다시 리시브날려주기
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
		//보낼길이와 보낸길이 0으로 초기화
		_info->packet->Comp_Sendbytes = _info->packet->Sendbytes = 0;
		//다 보냈으니 유저의 큐에 있는 정보 팝
		_info->queue.Pop();
		//유저의 큐에 아직 내용이 있을 경우
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
		//WSASend 다시 호출
		retval = IOCP_Send(_info);
		if (retval == TRANSSTATE::DISCONNECT)
		{
			RemoveSocketInfo(_info);
		}
	}
}

//iocp용 wsarecv함수
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

//iocp용 wsasend함수
int IOCP_Send(SOCKETINFO* _info)
{
	WSABUF wsabuf;
	ZeroMemory(&wsabuf, sizeof(wsabuf));
	//유저 큐의 맨 앞에 있는 미션을 mission에 넣기
	Mission* mission = _info->queue.Front();
	//유저가 보낼 총 길이 = 미션의 size
	_info->packet->Sendbytes = mission->size;
	//유저의 쓰기오버랩의 오버랩구조체 초기화
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
		(char*)&_totalsize,// 토탈사이즈 
		sizeof(int)// 토탈사이즈 크기 
		);

	memcpy
		(
		_buf + sizeof(int),// buf+sizeof(int) 
		(char*)&_protocal,// 프로토콜 값 
		sizeof(PROTOCAL)// 프로토콜 크기 
		);

	memcpy
		(
		_buf + sizeof(int)+sizeof(PROTOCAL),// buf+sizeof(int)+sizeof(PROTOCAL) 
		(char*)&_size,// 넣을문자길이 
		sizeof(int)// 문자크기 
		);

	memcpy
		(
		_buf + sizeof(int)+sizeof(PROTOCAL)+sizeof(int),// buf+sizeof(int)+sizeof(PROTOCAL)+sizeof(int) 
		_data,// 넣을 문자내용 
		_size// 문자길이 
		);
	return true;
}

BOOL UnPacking(SOCKETINFO* _info, PROTOCAL* _protocal)
{
	memcpy
		(
		_protocal,// 프로토콜 
		_info->packet->RecvBuf + sizeof(int),// _info->packet->RecvBuf + sizeof(int) 
		sizeof(PROTOCAL)// 프로토콜 크기 
		);

	memcpy
		(
		&_info->packet->len,// _info->circle.pos의 길이 
		_info->packet->RecvBuf + sizeof(int)+sizeof(PROTOCAL),// _info->packet->RecvBuf + sizeof(int) + sizeof(PROTOCAL) 
		sizeof(int)// 인트형 크기 
		);

	memcpy
		(
		_info->packet->RecvBuf,// 버퍼 
		_info->packet->RecvBuf + sizeof(int)+sizeof(PROTOCAL)+sizeof(int),// _info->packet->RecvBuf + sizeof(int) + sizeof(PROTOCAL) + sizeof(int) 
		_info->packet->len// 길이 
		);
	//printf("[recv] x : %d, y : %d\n", _info->circle.pos.x, _info->circle.pos.y);
	return true;
}

int PushQueue(SOCKETINFO* _info, Mission* _mission)
{
	//유저의 큐에 미션을 넣기전에 큐가 비었는지 확인
	if (_info->queue.QIsEmpty() == true)
	{
		//유저의 큐에 미션을 넣어준다.
		_info->queue.Push(_mission);
		//큐에 내용물이 한개이므로 바로 send를 날려준다
		return IOCP_Send(_info);
	}
	//큐가 비어있지 않고 내용물이 있으면 그냥 넣어주기만 한다
	else
	{
		_info->queue.Push(_mission);
	}
	return TRANSSTATE::SOC_TRUE;
}

BOOL CheckRecv(SOCKETINFO* _info)
{
	//맨처음 총 받은 길이가 0이고 현재까지 받은 길이가 sizeof(int)보다 크거가 같을 경우
	if (_info->packet->Recvbytes == 0)
	{
		//다 받았을 경우
		if (_info->packet->Comp_Recvbytes >= sizeof(int))
		{
			//앞에 토탈사이즈를 _info->packet->Recvbytes에 넣어준다
			memcpy(&_info->packet->Recvbytes, _info->packet->RecvBuf, sizeof(int));
		}
	}
	//총 받을 길이가 0이 아닐 경우(맨처음 4바이트인 토탈사이즈를 받아서 그 값이 저장되어 있을 경우

	//현재 받은 길이와 받을 길이가 같을 경우(리시브가 끝난 경우)
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
	//다 보냈을 경우
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