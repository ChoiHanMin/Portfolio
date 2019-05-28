#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXUSER 10
#define SERVERPORT 9000
#define SERVERIP "127.0.0.1"
#define BUFSIZE 512
#define SPEED 10

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("원이동");


enum PROTOCAL
{
	NONE_USER,		// 유저가 아무작업도 안할때 
	CREATE_USER,	// 새로 유저가 들어왔을때 
	SETTING_USER,	// 새로들어온 유저의 정보를 세팅해주고 보내줄때 
	MOVE_USER,		// 유저가 움직였을때 
	UPDATE_USER,	// 바뀐정보를 알려줄때 
	DELETE_USER		// 유저가 나갔을때 
};

struct CIRCLE		// 원 정보 구조체 
{
	int idnum;		// 유저고유번호 
	POINT pos;		// 유저좌표 
	int radius;		// 유저 반지름 
	COLORREF color;	// 유저 RGB색 
	bool login;		// 유저가 접속중인 상태 
};

CIRCLE MyCircle;					// 나 자신
CIRCLE user[MAXUSER];				// 유저배열 
int UserCount;						// 유저 카운트 
RECT rt;							// 영역
SOCKET server_sock;					//  소켓
HANDLE hSendEvent;					// 읽기쓰레드
int retval;
bool clicked;						// 맨처음 클릭했는지 안했는지 체크
PROTOCAL protocal;
CRITICAL_SECTION cs;
HWND hwnd;

//  작업자 스레드 함수
DWORD CALLBACK ProcessThread(LPVOID _arg);
// 리시브전용 스레드 함수
DWORD CALLBACK RecvThread(LPVOID _arg);

// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg);
// 소켓함수오류출력
void err_display(char* msg);
// 사용자 정의 데이터 수신함수
int recvn(SOCKET s, char* buf, int len, int flags);
// 패킹함수
BOOL Packing(int _totalsize, char* _buf, PROTOCAL _protocal, int _size, char* _data);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH MyBrush, OldBrush;

	switch (iMessage)
	{
	case WM_CREATE:
		// rect 설정
		GetClientRect(hWnd, &rt);
		hwnd = hWnd;
		clicked = false;
		// 이벤트 설정
		hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		// 총 스레드 생성
		CloseHandle(CreateThread(NULL, 0, ProcessThread, NULL, 0, NULL)); // 메모리 누수때문에 쓰레드를 바로 닫아주지만 쓰레드 영향은 없다.

		break;

		// 마우스왼쪽버튼 클릭했을때
	case WM_LBUTTONDOWN:
		// clicked가 true이면 이미 마우스버튼을 한번이라도 눌렀기 때문에 break로 빠져나간다
		if (clicked == false)
		{
			// 좌표값 저장
			MyCircle.pos.x = LOWORD(lParam);
			MyCircle.pos.y = HIWORD(lParam);
			clicked = true;
			protocal = PROTOCAL::CREATE_USER;
			// SendEvent켜주기
			SetEvent(hSendEvent);
		}
		break;
		// 키보드버튼을 눌렀을때
	case WM_KEYDOWN:
		// clicked가 true이면 화면에 이미 원이 그려져있는 상태
		if (clicked == true)
		{
			switch (wParam)
			{
				// 방향키 왼쪽을 눌렀을 때
			case VK_LEFT:
				MyCircle.pos.x -= SPEED;
				break;
				// 방향키 오른쪽을 눌렀을 때
			case VK_RIGHT:
				MyCircle.pos.x += SPEED;
				break;
				// 방향키 위를 눌렀을 때
			case VK_UP:
				MyCircle.pos.y -= SPEED;
				break;
				// 방향키 아래를 눌렀을 때
			case VK_DOWN:
				MyCircle.pos.y += SPEED;
				break;
			}
			protocal = PROTOCAL::MOVE_USER;
			SetEvent(hSendEvent);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		for (int i = 0; i < UserCount; i++)
		{
			CIRCLE circle = user[i];
			MyBrush = (HBRUSH)CreateSolidBrush(circle.color);
			OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);

			Ellipse(hdc, circle.pos.x - circle.radius - MyCircle.pos.x + rt.right / 2, circle.pos.y - circle.radius - MyCircle.pos.y + rt.bottom / 2, circle.pos.x + circle.radius - MyCircle.pos.x + rt.right / 2, circle.pos.y + circle.radius - MyCircle.pos.y + rt.bottom / 2);

			DeleteObject(OldBrush);
			DeleteObject(MyBrush);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

// 작업자 스레드
DWORD CALLBACK ProcessThread(LPVOID _arg)
{
	InitializeCriticalSection(&cs);
	//  윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);

	retval = connect(server_sock, (SOCKADDR*)&serveraddr, sizeof(SOCKADDR));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	CloseHandle(CreateThread(NULL, 0, RecvThread, NULL, 0, NULL));

	int totalsize;
	char buf[BUFSIZE];


	while (true)
	{
		// 샌드이벤트 기다려주기
		WaitForSingleObject(hSendEvent, INFINITE);
		switch (protocal)
		{
		case PROTOCAL::CREATE_USER:
			totalsize = sizeof(int)+sizeof(PROTOCAL)+sizeof(int)+sizeof(MyCircle.pos);
			Packing(totalsize, buf, PROTOCAL::CREATE_USER, sizeof(MyCircle.pos), (char*)&MyCircle.pos);
			retval = send(server_sock, buf, totalsize, 0);
			if (retval == SOCKET_ERROR)
			{
				err_quit("Send()");
			}
			break;
		case PROTOCAL::MOVE_USER:
			totalsize = sizeof(int)+sizeof(PROTOCAL)+sizeof(int)+sizeof(MyCircle.pos);
			Packing(totalsize, buf, PROTOCAL::MOVE_USER, sizeof(MyCircle.pos), (char*)&MyCircle.pos);
			retval = send(server_sock, buf, totalsize, 0);
			if (retval == SOCKET_ERROR)
			{
				err_quit("Send()");
			}
			break;
		}
		InvalidateRect(hwnd, NULL, TRUE);
	}
	DeleteCriticalSection(&cs);
	return 0;
}

// 리시브전용 스레드 함수
DWORD CALLBACK RecvThread(LPVOID _arg)
{
	int totalsize;
	int size;
	bool flag = false;
	char buf[BUFSIZE];
	char msgbuf[BUFSIZE];
	CIRCLE temp;
	PROTOCAL protocal;

	while (true)
	{
		ZeroMemory(buf, sizeof(buf));

		retval = recvn(server_sock, (char*)&totalsize, sizeof(int), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("Recvn() Error");
			return 0;
		}
		/////////////////

		retval = recvn(server_sock, buf, totalsize - sizeof(int), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("Recvn() Error");
			return 0;
		}

		memcpy(&protocal, buf, sizeof(PROTOCAL));
		memcpy(&size, buf + sizeof(PROTOCAL), sizeof(int));
		memcpy(&temp, buf + sizeof(PROTOCAL)+sizeof(int), size);

		switch (protocal)
		{
		case PROTOCAL::SETTING_USER:
			MyCircle = temp;
			break;
		case PROTOCAL::UPDATE_USER:
			EnterCriticalSection(&cs);
			for (int i = 0; i < UserCount; i++)
			{
				if (user[i].idnum == temp.idnum)
				{
					user[i].pos = temp.pos;
					flag = true;
					break;
				}
			}
			if (flag == false)
			{
				user[UserCount++] = temp;
			}
			LeaveCriticalSection(&cs);
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		case PROTOCAL::DELETE_USER:
			int index;
			EnterCriticalSection(&cs);
			for (int i = 0; i < UserCount; i++)
			{
				if (user[i].idnum == temp.idnum)
				{
					index = i;
					break;
				}
			}
			for (; index < UserCount - 1; index++)
			{
				user[index] = user[index + 1];
			}
			UserCount--;
			LeaveCriticalSection(&cs);
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		flag = false;
	}
	return 0;
}


// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg)
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

// 소켓함수오류출력
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 사용자 정의 데이터 수신함수
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		else if (received == 0)
		{
			break;
		}
		left -= received;
		ptr += received;
	}
	return (len - left);
}

BOOL Packing(int _totalsize, char* _buf, PROTOCAL _protocal, int _size, char* _data)
{
	ZeroMemory(_buf, sizeof(_buf));
	memcpy
		(
		_buf,// buf 
		(char*)&_totalsize,						// 토탈사이즈 
		sizeof(int)								// 토탈사이즈 크기 
		);

	memcpy
		(
		_buf + sizeof(int),						// buf+sizeof(int) 
		(char*)&_protocal,						// 프로토콜 값 
		sizeof(PROTOCAL)						// 프로토콜 크기 
		);

	memcpy
		(
		_buf + sizeof(int)+sizeof(PROTOCAL),  // buf+sizeof(int)+sizeof(PROTOCAL) 
		(char*)&_size,						  // 넣을문자길이 
		sizeof(int)							  // 문자크기 
		);

	memcpy
		(
		_buf + sizeof(int)+sizeof(PROTOCAL)+sizeof(int),	// buf+sizeof(int)+sizeof(PROTOCAL)+sizeof(int) 
		_data,												// 넣을 문자내용 
		_size												// 문자길이 
		);
	return true;
}