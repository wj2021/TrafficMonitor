// TaskBarDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TaskBarDlg.h"
#include "afxdialogex.h"


// CTaskBarDlg �Ի���

IMPLEMENT_DYNAMIC(CTaskBarDlg, CDialogEx)

CTaskBarDlg::CTaskBarDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TASK_BAR_DIALOG, pParent)
{

}

CTaskBarDlg::~CTaskBarDlg()
{
}

void CTaskBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTaskBarDlg, CDialogEx)
	ON_WM_RBUTTONUP()
	ON_WM_INITMENU()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTaskBarDlg ��Ϣ��������


void CTaskBarDlg::ShowInfo()
{
	if (this->m_hWnd == NULL || m_pDC == nullptr) return;
	CString str;
	CString in_speed = CCommon::DataSizeToString(theApp.m_in_speed);
	CString out_speed = CCommon::DataSizeToString(theApp.m_out_speed);
	
	//���û����DC
	CDC MemDC;
	CBitmap MemBitmap;
	MemDC.CreateCompatibleDC(NULL);
	int draw_width = (theApp.m_tbar_show_cpu_memory ? m_window_width : m_window_width_s);
	MemBitmap.CreateCompatibleBitmap(m_pDC, draw_width, m_rect.Height());
	MemDC.SelectObject(&MemBitmap);
	//��ͼ
	CRect tmp{ m_rect };
	tmp.MoveToXY(0, 0);
	MemDC.FillSolidRect(tmp, theApp.m_taskbar_data.back_color);		//��䱳��ɫ
	MemDC.SelectObject(&m_font);
	//�����ϴ��ٶ�
	tmp.bottom = m_window_height / 2;
	tmp.right = tmp.left + m_window_width_s;
	if (!theApp.m_taskbar_data.swap_up_down)
		str.Format(_T("%s%s/s"), theApp.m_taskbar_data.up_string.c_str(), out_speed.GetString());
	else
		str.Format(_T("%s%s/s"), theApp.m_taskbar_data.down_string.c_str(), in_speed.GetString());
	CCommon::DrawWindowText(&MemDC, tmp, str, theApp.m_taskbar_data.text_color, theApp.m_taskbar_data.back_color);
	//���������ٶ�
	tmp.MoveToY(tmp.bottom);
	if (!theApp.m_taskbar_data.swap_up_down)
		str.Format(_T("%s%s/s"), theApp.m_taskbar_data.down_string.c_str(), in_speed.GetString());
	else
		str.Format(_T("%s%s/s"), theApp.m_taskbar_data.up_string.c_str(), out_speed.GetString());
	CCommon::DrawWindowText(&MemDC, tmp, str, theApp.m_taskbar_data.text_color, theApp.m_taskbar_data.back_color);
	if (theApp.m_tbar_show_cpu_memory)
	{
		//����CPU������
		if (m_taskbar_on_top_or_bottom)
		{
			tmp.MoveToXY(m_window_width_s, 0);
			tmp.right = tmp.left + (m_window_width - m_window_width_s);
		}
		else
		{
			tmp.MoveToY(m_window_height);
			tmp.right = tmp.left + (m_window_width - m_window_width_s);
		}
		str.Format(_T("%s%d%%"), theApp.m_taskbar_data.cpu_string.c_str(), theApp.m_cpu_usage);
		CCommon::DrawWindowText(&MemDC, tmp, str, theApp.m_taskbar_data.text_color, theApp.m_taskbar_data.back_color);
		//�����ڴ�������
		tmp.MoveToY(tmp.bottom);
		str.Format(_T("%s%d%%"), theApp.m_taskbar_data.memory_string.c_str(), theApp.m_memory_usage);
		CCommon::DrawWindowText(&MemDC, tmp, str, theApp.m_taskbar_data.text_color, theApp.m_taskbar_data.back_color);
	}
	//��������DC�е�ͼ�񿽱�����Ļ����ʾ
	m_pDC->BitBlt(0,0, draw_width, m_rect.Height(), &MemDC, 0, 0, SRCCOPY);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
}


bool CTaskBarDlg::AdjustWindowPos()
{
	if (this->m_hWnd == NULL)
		return false;
	CRect rcMin;
	::GetWindowRect(m_hMin, rcMin);	//�����С�����ڵ�����
	m_taskbar_on_top_or_bottom = IsTaskbarOnTopOrBottom();
	if (m_taskbar_on_top_or_bottom)		//�������������涥����ײ�ʱ
	{
		//���ô��ڴ�С
		m_rect.right = m_rect.left + (theApp.m_tbar_show_cpu_memory ? m_window_width : m_window_width_s);
		m_rect.bottom = m_rect.top + m_window_height;
		if (rcMin.Width() != m_min_bar_width)	//�����С�����ڵĿ��ȸı��ˣ������������������ڵ�λ��
		{
			m_rcMin = rcMin;
			m_min_bar_width = m_rcMin.Width() - m_rect.Width();	//������С�����ڿ���
			::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);	//������С�����ڵ�λ��
			::MoveWindow(this->m_hWnd, m_left_space + m_rcMin.Width() - m_rect.Width() + 2, (m_rcMin.Height() - m_rect.Height()) / 2, \
				m_rect.Width(), m_rect.Height(), TRUE);		//�������������ڵ�λ��

			CRect rect;
			GetWindowRect(rect);	//��ȡ��ǰ���ڵľ���λ��
			//�������û�б��ɹ�Ƕ�뵽�������������ƶ����˻�����Ļ���Ͻǵľ���λ�ã����������ڵ�λ��
			if (rect.left == m_left_space + m_rcMin.Width() - m_rect.Width() + 2 && rect.top == (m_rcMin.Height() - m_rect.Height()) / 2)
			{
				rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
				this->MoveWindow(rect);
			}
		}
	}
	else		//������������Ļ�������Ҳ�ʱ
	{
		//���ô��ڴ�С
		m_rect.right = m_rect.left + m_window_width_s;
		m_rect.bottom = m_rect.top + (theApp.m_tbar_show_cpu_memory ? (2 * m_window_height) : m_window_height);
		if (rcMin.Height() != m_min_bar_height)	//�����С�����ڵĸ߶ȸı��ˣ������������������ڵ�λ��
		{
			m_rcMin = rcMin;
			m_min_bar_height = m_rcMin.Height() - m_rect.Height();	//������С�����ڸ߶�
			::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);	//������С�����ڵ�λ��
			int left_pos = (m_rcMin.Width() - m_window_width_s) / 2;
			if (left_pos < 0) left_pos = 0;
			::MoveWindow(this->m_hWnd, left_pos, m_top_space + m_rcMin.Height() - m_rect.Height() + 2, \
				m_rect.Width(), m_rect.Height(), TRUE);		//�������������ڵ�λ��

			CRect rect;
			GetWindowRect(rect);	//��ȡ��ǰ���ڵľ���λ��
			//�������û�б��ɹ�Ƕ�뵽�������������ƶ����˻�����Ļ���Ͻǵľ���λ�ã����������ڵ�λ��
			if (rect.left == left_pos && rect.top == m_top_space + m_rcMin.Height() - m_rect.Height() + 2)
			{
				rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
				this->MoveWindow(rect);
			}
		}
	}
	if (m_connot_insert_to_task_bar && ::GetForegroundWindow() == m_hTaskbar)	//�ڴ����޷�Ƕ��������ʱ������������������������ϣ����ô����ö�
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);			//�����ö�
	}
	return true;
}


bool CTaskBarDlg::IsTaskbarOnTopOrBottom()
{
	CRect rect;
	CRect rcMin;
	CRect rcBar;
	if (m_hTaskbar != 0)
	{
		::GetWindowRect(m_hMin, rcMin);	//�����С�����ڵ�����
		::GetWindowRect(m_hBar, rcBar);	//��ö�������������
		m_left_space = rcMin.left - rcBar.left;
		m_top_space = rcMin.top - rcBar.top;

		::GetWindowRect(m_hTaskbar, rect);			//��ȡ�������ľ�������
		return (rect.Width()>=rect.Height());		//����������Ŀ��ȴ��ڸ߶ȣ�����������Ļ�Ķ�����ײ�
	}
	else
	{
		return true;
	}
}

void CTaskBarDlg::SaveConfig()
{
	CCommon::WritePrivateProfileIntW(L"task_bar", L"task_bar_back_color", theApp.m_taskbar_data.back_color, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"task_bar", L"task_bar_text_color", theApp.m_taskbar_data.text_color, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"task_bar", L"tack_bar_show_cpu_memory", theApp.m_tbar_show_cpu_memory, theApp.m_config_path.c_str());
	WritePrivateProfileString(_T("task_bar"), _T("tack_bar_font_name"), theApp.m_taskbar_data.font_name, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"task_bar", L"tack_bar_font_size", theApp.m_taskbar_data.font_size, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"task_bar", L"task_bar_swap_up_down", theApp.m_taskbar_data.swap_up_down, theApp.m_config_path.c_str());

	WritePrivateProfileString(_T("task_bar"), _T("up_string"), (theApp.m_taskbar_data.up_string + L'$').c_str(), theApp.m_config_path.c_str());
	WritePrivateProfileString(_T("task_bar"), _T("down_string"), (theApp.m_taskbar_data.down_string + L'$').c_str(), theApp.m_config_path.c_str());
	WritePrivateProfileString(_T("task_bar"), _T("cpu_string"), (theApp.m_taskbar_data.cpu_string + L'$').c_str(), theApp.m_config_path.c_str());
	WritePrivateProfileString(_T("task_bar"), _T("memory_string"), (theApp.m_taskbar_data.memory_string + L'$').c_str(), theApp.m_config_path.c_str());
}

void CTaskBarDlg::LoadConfig()
{
	theApp.m_taskbar_data.back_color = GetPrivateProfileInt(_T("task_bar"), _T("task_bar_back_color"), 0, theApp.m_config_path.c_str());
	theApp.m_taskbar_data.text_color = GetPrivateProfileInt(_T("task_bar"), _T("task_bar_text_color"), 0x00ffffffU, theApp.m_config_path.c_str());
	theApp.m_tbar_show_cpu_memory = (GetPrivateProfileInt(_T("task_bar"), _T("tack_bar_show_cpu_memory"), 1, theApp.m_config_path.c_str()) != 0);
	theApp.m_taskbar_data.swap_up_down = (GetPrivateProfileInt(_T("task_bar"), _T("task_bar_swap_up_down"), 0, theApp.m_config_path.c_str()) != 0);
	GetPrivateProfileString(_T("task_bar"), _T("tack_bar_font_name"), _T("΢���ź�"), theApp.m_taskbar_data.font_name.GetBuffer(32), 32, theApp.m_config_path.c_str());
	theApp.m_taskbar_data.font_size = GetPrivateProfileInt(_T("task_bar"), _T("tack_bar_font_size"), 9, theApp.m_config_path.c_str());
	
	wchar_t buff[256];
	GetPrivateProfileStringW(L"task_bar", L"up_string", L"��: $", buff, 256, theApp.m_config_path.c_str());
	theApp.m_taskbar_data.up_string = buff;
	if (!theApp.m_taskbar_data.up_string.empty() && theApp.m_taskbar_data.up_string.back() == L'$')		//�����ȡ���ַ���ĩβ��'$'�������ɾ�������ڽ����ȡini�ļ�������ַ���ʱ�޷�Ӯȡĩβ�Ŀո�����⣬��ʱֻҪ��ĩβ����'$'����
		theApp.m_taskbar_data.up_string.pop_back();
	GetPrivateProfileStringW(L"task_bar", L"down_string", L"��: $", buff, 256, theApp.m_config_path.c_str());
	theApp.m_taskbar_data.down_string = buff;
	if (!theApp.m_taskbar_data.down_string.empty() && theApp.m_taskbar_data.down_string.back() == L'$')
		theApp.m_taskbar_data.down_string.pop_back();
	GetPrivateProfileStringW(L"task_bar", L"cpu_string", L"CPU: $", buff, 256, theApp.m_config_path.c_str());
	theApp.m_taskbar_data.cpu_string = buff;
	if (!theApp.m_taskbar_data.cpu_string.empty() && theApp.m_taskbar_data.cpu_string.back() == L'$')
		theApp.m_taskbar_data.cpu_string.pop_back();
	GetPrivateProfileStringW(L"task_bar", L"memory_string", L"�ڴ�: $", buff, 256, theApp.m_config_path.c_str());
	theApp.m_taskbar_data.memory_string = buff;
	if (!theApp.m_taskbar_data.memory_string.empty() && theApp.m_taskbar_data.memory_string.back() == L'$')
		theApp.m_taskbar_data.memory_string.pop_back();
}

void CTaskBarDlg::ApplySettings()
{
	//���m_font�Ѿ�������һ��������Դ�������ͷ���
	if (m_font.m_hObject)
	{
		m_font.DeleteObject();
	}
	//�����µ�����
	m_font.CreatePointFont(theApp.m_taskbar_data.font_size * 10, theApp.m_taskbar_data.font_name);

	CalculateWindowWidth();
}

void CTaskBarDlg::CalculateWindowWidth()
{
	//������ʾ�ϴ����ز�������Ҫ�Ŀ���
	CString str1, str2;
	int width1, width2;
	str1.Format(_T("%s8888.8KB/s"), theApp.m_taskbar_data.up_string.c_str());
	str2.Format(_T("%s8888.8KB/s"), theApp.m_taskbar_data.down_string.c_str());
	width1= m_pDC->GetTextExtent(str1).cx;		//����ʹ�õ�ǰ������ʾ�ı���Ҫ�Ŀ���ֵ
	width2= m_pDC->GetTextExtent(str2).cx;
	m_window_width_s = max(width1, width2);

	//������ʾCPU���ڴ沿������Ҫ�Ŀ���
	int width_cpu_memory;
	str1.Format(_T("%s100%%"), theApp.m_taskbar_data.cpu_string.c_str());
	str2.Format(_T("%s100%%"), theApp.m_taskbar_data.memory_string.c_str());
	width1 = m_pDC->GetTextExtent(str1).cx;
	width2 = m_pDC->GetTextExtent(str2).cx;
	width_cpu_memory = max(width1, width2);
	m_window_width = m_window_width_s + width_cpu_memory;
}

void CTaskBarDlg::SetToolTipsTopMost()
{
	m_tool_tips.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CTaskBarDlg::UpdateToolTips()
{
	CString tip_info;
	tip_info = CCommon::GetMouseTipsInfo(theApp.m_today_traffic, theApp.m_cpu_usage, theApp.m_memory_usage, theApp.m_used_memory, theApp.m_total_memory, theApp.m_tbar_show_cpu_memory);
	m_tool_tips.UpdateTipText(tip_info, this);
}

BOOL CTaskBarDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ����Ӷ���ĳ�ʼ��
	//��������������ͼ��
	ModifyStyleEx(0, WS_EX_TOOLWINDOW);

	LoadConfig();

	m_pDC = GetDC();

	//��������
	m_font.CreatePointFont(theApp.m_taskbar_data.font_size * 10, theApp.m_taskbar_data.font_name);
	m_pDC->SelectObject(&m_font);

	this->GetWindowRect(m_rect);
	//m_window_width = m_rect.Width();
	//m_window_width_s = static_cast<int>(m_rect.Width()*0.566f + 0.5f);
	CalculateWindowWidth();
	m_window_height = m_rect.Height();
	if (!theApp.m_tbar_show_cpu_memory)
		m_rect.right = m_rect.left + m_window_width_s;
	else
		m_rect.right = m_rect.left + m_window_width;

	m_hTaskbar = ::FindWindow(L"Shell_TrayWnd", NULL);		//Ѱ��������Shell_TrayWnd�Ĵ��ھ��
	m_hBar = ::FindWindowEx(m_hTaskbar, 0, L"ReBarWindow32", NULL);	//Ѱ�Ҷ��������ľ��
	m_hMin = ::FindWindowEx(m_hBar, 0, L"MSTaskSwWClass", NULL);	//Ѱ����С�����ڵľ��

	::GetWindowRect(m_hMin, m_rcMin);	//�����С�����ڵ�����
	::GetWindowRect(m_hBar, m_rcBar);	//��ö�������������
	m_left_space = m_rcMin.left - m_rcBar.left;
	m_top_space = m_rcMin.top - m_rcBar.top;

	m_taskbar_on_top_or_bottom = IsTaskbarOnTopOrBottom();
	if (m_taskbar_on_top_or_bottom)		//��������������涥����ײ�
	{
		m_min_bar_width = m_rcMin.Width() - m_rect.Width();	//������С�����ڿ���

		//ͨ����MoveWindow�������ı�С�����ڵĿ���
		::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);

		::SetParent(this->m_hWnd, m_hBar);	//�ѳ��򴰿����ó����������Ӵ���

		//�������򴰿ڵĴ�С��λ��
		//ע�����ѵ�ǰ��������Ϊ���������Ӵ��ں�MoveWindow�����ƶ���λ���ǻ��������������λ�ã�
		//��ĳЩ����£��类��ȫ������ֹʱ�������޷�Ƕ�������������ڻ��ƶ���������Ļ���Ͻǵľ���λ�á�
		::MoveWindow(this->m_hWnd, m_left_space + m_rcMin.Width() - m_rect.Width() + 2, (m_rcBar.Height() - m_rect.Height()) / 2, \
			m_rect.Width(), m_rect.Height(), TRUE);

		CRect rect;
		GetWindowRect(rect);	//��ȡ��ǰ���ڵľ���λ��
		//�������û�б��ɹ�Ƕ�뵽�������������ƶ����˻�����Ļ���Ͻǵľ���λ�ã����������ڵ�λ��
		if (rect.left == m_left_space + m_rcMin.Width() - m_rect.Width() + 2 && rect.top == (m_rcBar.Height() - m_rect.Height()) / 2)
		{
			rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
			this->MoveWindow(rect);
			m_connot_insert_to_task_bar = true;
			//MessageBox(_T("���棺����û�гɹ�Ƕ���������������ѱ���ȫ������ֹ��"), NULL, MB_ICONWARNING);
		}
	}
	else	//�������������������Ҳ�ʱ
	{
		int left_pos = (m_rcMin.Width() - m_window_width_s) / 2;
		if (left_pos < 0) left_pos = 0;
		if (theApp.m_tbar_show_cpu_memory)	//��CPU���ڴ������ʷŵ����ٵ�����
		{
			m_rect.right = m_rect.left + m_window_width_s;
			m_rect.bottom = m_rect.top + m_rect.Height() * 2;
		}
		m_min_bar_height = m_rcMin.Height() - m_rect.Height();	//������С�����ڸ߶�

		//ͨ����MoveWindow�������ı�С�����ڵĸ߶�
		::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);

		::SetParent(this->m_hWnd, m_hBar);	//�ѳ��򴰿����ó����������Ӵ���

		//�������򴰿ڵĴ�С��λ��
		//ע�����ѵ�ǰ��������Ϊ���������Ӵ��ں�MoveWindow�����ƶ���λ���ǻ��������������λ�ã�
		//��ĳЩ����£��类��ȫ������ֹʱ�������޷�Ƕ�������������ڻ��ƶ���������Ļ���Ͻǵľ���λ�á�
		::MoveWindow(this->m_hWnd, left_pos, m_top_space + m_rcMin.Height() - m_rect.Height() + 2, \
			m_rect.Width(), m_rect.Height(), TRUE);

		CRect rect;
		GetWindowRect(rect);	//��ȡ��ǰ���ڵľ���λ��
		//�������û�б��ɹ�Ƕ�뵽�������������ƶ����˻�����Ļ���Ͻǵľ���λ�ã����������ڵ�λ��
		if (rect.left == left_pos && rect.top == m_top_space + m_rcMin.Height() - m_rect.Height() + 2)
		{
			rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
			this->MoveWindow(rect);
			m_connot_insert_to_task_bar = true;
			//MessageBox(_T("���棺����û�гɹ�Ƕ���������������ѱ���ȫ������ֹ��"), NULL, MB_ICONWARNING);
		}
	}

	m_menu.LoadMenu(IDR_TASK_BAR_MENU);

	SetBackgroundColor(theApp.m_taskbar_data.back_color);

	//��ʼ�������ʾ
	m_tool_tips.Create(this, TTS_ALWAYSTIP);
	m_tool_tips.SetMaxTipWidth(600);
	m_tool_tips.AddTool(this, _T(""));
	SetToolTipsTopMost();		//������ʾ��Ϣ�����ö�

	SetTimer(TASKBAR_TIMER, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CTaskBarDlg::OnCancel()
{
	// TODO: �ڴ�����ר�ô����/����û���
	SaveConfig();
	DestroyWindow();
	//����رյ�ʱ�򣬰���С�����ڵ�width�ָ���ȥ
	if(IsTaskbarOnTopOrBottom())
		::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width(), m_rcMin.Height(), TRUE);
	else
		::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height(), TRUE);

	//CDialogEx::OnCancel();
}


void CTaskBarDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ

	CPoint point1;	//����һ������ȷ�����λ�õ�λ��  
	GetCursorPos(&point1);	//��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը�����  
	m_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //��ָ��λ����ʾ�����˵�
	CDialogEx::OnRButtonUp(nFlags, point1);
}


void CTaskBarDlg::OnInitMenu(CMenu* pMenu)
{
	CDialogEx::OnInitMenu(pMenu);

	// TODO: �ڴ˴�������Ϣ�����������
	pMenu->CheckMenuItem(ID_SHOW_CPU_MEMORY2, MF_BYCOMMAND | (theApp.m_tbar_show_cpu_memory ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_HIDE_MAIN_WND, MF_BYCOMMAND | (theApp.m_hide_main_window ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | (theApp.m_show_notify_icon ? MF_CHECKED : MF_UNCHECKED));

	pMenu->SetDefaultItem(ID_NETWORK_INFO);
}


BOOL CTaskBarDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ�����ר�ô����/����û���
	//���ΰ��س�����ESC���˳�
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;

	if (m_tool_tips.GetSafeHwnd() && (pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_LBUTTONUP ||
		pMsg->message == WM_MOUSEMOVE))
	{
		m_tool_tips.RelayEvent(pMsg);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CTaskBarDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ

	CDialogEx::OnMouseMove(nFlags, point);
}


void CTaskBarDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
	SendMessage(WM_COMMAND,ID_NETWORK_INFO);		//˫���󵯳����������顱�Ի���
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CTaskBarDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
	if (nIDEvent == TASKBAR_TIMER)
	{
		AdjustWindowPos();
		ShowInfo();
	}

	CDialogEx::OnTimer(nIDEvent);
}