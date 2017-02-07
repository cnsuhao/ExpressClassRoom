#include "LoginWnd.h"


LoginWnd::LoginWnd()
{
}


LoginWnd::~LoginWnd()
{
}

LPCTSTR LoginWnd::GetWindowClassName()const
{
	return _T("LoginWnd");
}

CDuiString LoginWnd::GetSkinFile()
{
	return _T("LoginWnd.xml");
}

void LoginWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_close"))
		{
			this->Close(0);
		}
		else if (msg.pSender->GetName() == _T("btn_login"))
		{
			CTabLayoutUI *tab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TabLay")));
			tab->SelectItem(1);
		}
		else if (msg.pSender->GetName() == _T("btn_cancel"))
		{
			CTabLayoutUI *tab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TabLay")));
			tab->SelectItem(0);
		}
		else if (msg.pSender->GetName() == _T("btn_rem"))
		{
			COptionUI* optrem = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_rem")));
			optrem->Selected(!optrem->IsSelected());
			is_rem = optrem->IsSelected();
		}
		else if (msg.pSender->GetName() == _T("btn_auto"))
		{
			COptionUI* optauto = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_auto")));
			optauto->Selected(!optauto->IsSelected());
			is_auto = optauto->IsSelected();
		}
	}
}


CDuiString LoginWnd::GetSkinFolder()
{

	return _T("Skin");

}


LRESULT LoginWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{/*
	BOOL b;
	switch (wParam)
	{
	case WM_TIMER:
		OnTimer(uMsg, wParam, lParam,b);
	default:
		break;
	}
	return 0;*/
}