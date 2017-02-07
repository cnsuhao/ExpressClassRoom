#include "expresshead.h"

#include "login/LoginWnd.h"
#include <vld.h>


int  _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	//���̻���
	HANDLE Hmutex = CreateMutex(NULL, TRUE, _T("mainWindows"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND h = ::FindWindow(NULL, _T("mainWindows"));
		if (h)
		{
			::SetForegroundWindow(h);
		}
		CloseHandle(Hmutex);
		::MessageBox(0, _T("�ó�����������"), _T("��ʾ"), MB_OK);
		return EXIT_FAILURE;
	}

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
		return 0;

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

	LoginWnd *login = new LoginWnd();
	login->Create(NULL, _T("��¼"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	login->CenterWindow();
	login->ShowModal();
	::CoUninitialize();
	return EXIT_SUCCESS;

}