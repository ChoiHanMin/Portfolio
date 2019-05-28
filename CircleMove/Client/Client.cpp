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
LPCTSTR lpszClass = TEXT("���̵�");


enum PROTOCAL
{
	NONE_USER,		// ������ �ƹ��۾��� ���Ҷ� 
	CREATE_USER,	// ���� ������ �������� 
	SETTING_USER,	// ���ε��� ������ ������ �������ְ� �����ٶ� 
	MOVE_USER,		// ������ ���������� 
	UPDATE_USER,	// �ٲ������� �˷��ٶ� 
	DELETE_USER		// ������ �������� 
};

struct CIRCLE		// �� ���� ����ü 
{
	int idnum;		// ����������ȣ 
	POINT pos;		// ������ǥ 
	int radius;		// ���� ������ 
	COLORREF color;	// ���� RGB�� 
	bool login;		// ������ �������� ���� 
};

CIRCLE MyCircle;					// �� �ڽ�
CIRCLE user[MAXUSER];				// �����迭 
int UserCount;						// ���� ī��Ʈ 
RECT rt;							// ����
SOCKET server_sock;					//  ����
HANDLE hSendEvent;					// �б⾲����
int retval;
bool clicked;						// ��ó�� Ŭ���ߴ��� ���ߴ��� üũ
PROTOCAL protocal;
CRITICAL_SECTION cs;
HWND hwnd;

//  �۾��� ������ �Լ�
DWORD CALLBACK ProcessThread(LPVOID _arg);
// ���ú����� ������ �Լ�
DWORD CALLBACK RecvThread(LPVOID _arg);

// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg);
// �����Լ��������
void err_display(char* msg);
// ����� ���� ������ �����Լ�
int recvn(SOCKET s, char* buf, int len, int flags);
// ��ŷ�Լ�
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
		// rect ����
		GetClientRect(hWnd, &rt);
		hwnd = hWnd;
		clicked = false;
		// �̺�Ʈ ����
		hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		// �� ������ ����
		CloseHandle(CreateThread(NULL, 0, ProcessThread, NULL, 0, NULL)); // �޸� ���������� �����带 �ٷ� �ݾ������� ������ ������ ����.

		break;

		// ���콺���ʹ�ư Ŭ��������
	case WM_LBUTTONDOWN:
		// clicked�� true�̸� �̹� ���콺��ư�� �ѹ��̶� ������ ������ break�� ����������
		if (clicked == false)
		{
			// ��ǥ�� ����
			MyCircle.pos.x = LOWORD(lParam);
			MyCircle.pos.y = HIWORD(lParam);
			clicked = true;
			protocal = PROTOCAL::CREATE_USER;
			// SendEvent���ֱ�
			SetEvent(hSendEvent);
		}
		break;
		// Ű�����ư�� ��������
	case WM_KEYDOWN:
		// clicked�� true�̸� ȭ�鿡 �̹� ���� �׷����ִ� ����
		if (clicked == true)
		{
			switch (wParam)
			{
				// ����Ű ������ ������ ��
			case VK_LEFT:
				MyCircle.pos.x -= SPEED;
				break;
				// ����Ű �������� ������ ��
			case VK_RIGHT:
				MyCircle.pos.x += SPEED;
				break;
				// ����Ű ���� ������ ��
			case VK_UP:
				MyCircle.pos.y -= SPEED;
				break;
				// ����Ű �Ʒ��� ������ ��
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

// �۾��� ������
DWORD CALLBACK ProcessThread(LPVOID _arg)
{
	InitializeCriticalSection(&cs);
	//  ���� �ʱ�ȭ
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
		// �����̺�Ʈ ��ٷ��ֱ�
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

// ���ú����� ������ �Լ�
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


// ���� �Լ� ���� ��� �� ����
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

// �����Լ��������
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

// ����� ���� ������ �����Լ�
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
		(char*)&_totalsize,						// ��Ż������ 
		sizeof(int)								// ��Ż������ ũ�� 
		);

	memcpy
		(
		_buf + sizeof(int),						// buf+sizeof(int) 
		(char*)&_protocal,						// �������� �� 
		sizeof(PROTOCAL)						// �������� ũ�� 
		);

	memcpy
		(
		_buf + sizeof(int)+sizeof(PROTOCAL),  // buf+sizeof(int)+sizeof(PROTOCAL) 
		(char*)&_size,						  // �������ڱ��� 
		sizeof(int)							  // ����ũ�� 
		);

	memcpy
		(
		_buf + sizeof(int)+sizeof(PROTOCAL)+sizeof(int),	// buf+sizeof(int)+sizeof(PROTOCAL)+sizeof(int) 
		_data,												// ���� ���ڳ��� 
		_size												// ���ڱ��� 
		);
	return true;
}