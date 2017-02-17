#include <time.h>
#include "NotifyWnd.h"

#define  TIMER_ID_LOGIN	1000




NotifyWnd::NotifyWnd(std::string msg, std::string title) :_msg(msg), _title(title)
{

}


NotifyWnd::~NotifyWnd()
{
}

LPCTSTR NotifyWnd::GetWindowClassName()const
{
	return _T("NotifyWnd");
}


CDuiString NotifyWnd::GetSkinFile()
{
	return _T("NotifyView.xml");
}

void NotifyWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_ok"))
		{
			this->Close(IDOK);
		}
		else if (msg.pSender->GetName() == _T("btn_cancel"))
		{
			this->Close(IDCANCEL);
		}
	}
}


CDuiString NotifyWnd::GetSkinFolder()
{
	return _T("Skin");
}

void NotifyWnd::Init()
{
	CLabelUI *labtip = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_tip")));
	labtip->SetText(_msg.c_str());
	CLabelUI *labtitle = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_title")));
	labtitle->SetText(_title.c_str());
	CButtonUI*btn_ok = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_ok")));
	CButtonUI*btn_cancel = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_cancel")));
}

LRESULT NotifyWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_TIMER)
		return OnTimer(uMsg, wParam, lParam);
	else
		return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT NotifyWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

int TipMsg::ShowMsgWindow(HWND parentHwnd, std::string msg, std::string title)
{
	NotifyWnd* pwnd = new NotifyWnd(msg,title);
	pwnd->Create(parentHwnd, _T("Tip"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
	pwnd->CenterWindow();
	return pwnd->ShowModal();
}