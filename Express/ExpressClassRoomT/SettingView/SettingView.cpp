#include "SettingView.h"

SettingView::SettingView() :cfg(NULL)
{
}


SettingView::~SettingView()
{
	if (cfg)
	{
		cfg->save();
		delete cfg;
		cfg = NULL;
	}
}


LPCTSTR SettingView::GetWindowClassName()const
{
	return _T("SettingView");
}

CDuiString SettingView::GetSkinFile()
{
	return _T("SettingView.xml");
}
CDuiString SettingView::GetSkinFolder()
{
	return _T("Skin");
}

void SettingView::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_close"))
		{
			this->Close();
		}
		else if (msg.pSender->GetName() == _T("btn_upload"))
		{
			;
		}
	}
	else if (msg.sType == DUI_MSGTYPE_RETURN || msg.sType == DUI_MSGTYPE_KILLFOCUS)
	{
		if (msg.pSender->GetName() == _T("edit_remote1"))
		{
			cfg->addValue("ip1", msg.pSender->GetText().GetData(),"remote");
		}
		else if (msg.pSender->GetName() == _T("edit_remote2"))
		{
			cfg->addValue("ip2", msg.pSender->GetText().GetData(), "remote");
		}
		else if (msg.pSender->GetName() == _T("edit_remote3"))
		{
			cfg->addValue("ip3", msg.pSender->GetText().GetData(), "remote");
		}
		else if (msg.pSender->GetName() == _T("edit_remote4"))
		{
			cfg->addValue("ip4", msg.pSender->GetText().GetData(), "remote");
		}
		else if (msg.pSender->GetName() == _T("edit_lubo_IP"))
		{
			cfg->addValue("luboip", msg.pSender->GetText().GetData(), "remote");
		}
		else if (msg.pSender->GetName() == _T("edit_cloud_IP"))
		{
			cfg->addValue("cloudip", msg.pSender->GetText().GetData(), "remote");
		}
	}

}

void SettingView::Init()
{
	cfg = new ConfigFile(CLOUD_IP_FILE);
	remoteIP[0] = cfg->getValue("ip1", "remote");
	remoteIP[1] = cfg->getValue("ip2", "remote");
	remoteIP[2] = cfg->getValue("ip3", "remote");
	remoteIP[3] = cfg->getValue("ip4", "remote");
	remoteIP[4] = cfg->getValue("luboip", "remote");
	remoteIP[5] = cfg->getValue("cloudip", "remote");

	localIP = cfg->getValue("localip", "local");
	name = cfg->getValue("name", "local");

	for (int i = 0; i < 4; i++)
	{
		char ctrl_name[200];
		sprintf(ctrl_name, "edit_remote%d", i + 1);
		remote_edit[i] = static_cast<CEditUI*>(m_PaintManager.FindControl(ctrl_name));
		if (!remoteIP[i].empty())
			remote_edit[i]->SetText(remoteIP[i].c_str());
	}

	remote_edit[4] = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_lubo_IP")));
	remote_edit[4]->SetText(remoteIP[4].c_str());
	remote_edit[5] = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_cloud_IP")));
	remote_edit[5]->SetText(remoteIP[5].c_str());

	CEditUI *edit_name = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_name")));
	//edit_name->SetText(CDuiCharConver::ANSIToUTF8(name).c_str());

	CLabelUI*lab_local = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_local_IP")));
	lab_local->SetText(localIP.c_str());
}