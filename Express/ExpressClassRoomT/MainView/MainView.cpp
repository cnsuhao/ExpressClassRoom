#include "MainView.h"
#include "../Video/Video.h"
#define  TIME_ID_UPDATE_TIME	1001

MainView::MainView() :lab_date(NULL), lab_time(NULL)
{
}
MainView::~MainView()
{

}

LPCSTR  MainView::GetWindowClassName() const
{
	return _T("MainView");
}

CDuiString MainView::GetSkinFolder()
{
	return _T("Skin");
}

CDuiString MainView::GetSkinFile()
{
	return _T("MainView.xml");
}

void MainView::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_min"))
		{
			::ShowWindow(*this, SW_MINIMIZE);
		}
		else if (msg.pSender->GetName() == _T("btn_max"))
		{
			static bool record = false;
			if (!record)
			{
				::ShowWindow(*this, SW_MAXIMIZE);
				record = !record;
			}
			else
			{
				::ShowWindow(*this, SW_RESTORE);
				record = !record;
			}
		}
		else if (msg.pSender->GetName() == _T("btn_close"))
		{
			Close();
		}
		else if (msg.pSender->GetName() == _T("btn_expend"))
		{
			CVerticalLayoutUI *ver = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("ClassRoomLay")));
			ver->SetVisible(!ver->IsVisible());
		}

	}
}

CControlUI* MainView::CreateControl(LPCTSTR pstrClass)
{
	if (pstrClass == _T("Video"))
	{
		return new CVideoUI();
	}
	else
		return __super::CreateControl(pstrClass);
}
LRESULT MainView::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_TIMER)
		return OnTimer(uMsg, wParam, lParam);
	else
		return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT MainView::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (wParam == TIME_ID_UPDATE_TIME)
	{
		DisplayDateTime();
	}
	return 0;
}
void MainView::Init()
{
	lab_date = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_date")));
	lab_time = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_time")));

	SetTimer(*this, TIME_ID_UPDATE_TIME, 999, NULL);

	CVideoUI *video = static_cast<CVideoUI*>(m_PaintManager.FindControl(_T("mainVideo")));
}
void MainView::DisplayDateTime()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	static char cdate[250];
	static char ctime[250];
	sprintf(cdate, "%d-%d-%d", st.wYear, st.wMonth, st.wDay);
	if (lab_date)
		lab_date->SetText(cdate);

	sprintf(ctime, "%d:%d:%d",st.wHour,st.wMinute,st.wSecond);

	if (lab_time)
		lab_time->SetText(ctime);

}