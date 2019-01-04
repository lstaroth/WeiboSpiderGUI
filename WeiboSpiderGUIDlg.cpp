
// WeiboSpiderGUIDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "WeiboSpiderGUI.h"
#include "WeiboSpiderGUIDlg.h"
#include "afxdialogex.h"
#include "LinkPython.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool Pyenvinit;
CString Url;
BOOL ChooseIndex = false;
CListBox ListResult;
CEdit CEditContent;
CProgressCtrl CProgressCtrlStatus;
HANDLE NewThread = nullptr;

// CWeiboSpiderGUIDlg 对话框

CWeiboSpiderGUIDlg::CWeiboSpiderGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WEIBOSPIDERGUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWeiboSpiderGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_URL, Url);
	DDX_Radio(pDX, IDC_RADIO_COMMENTS, ChooseIndex);
	DDX_Control(pDX, IDC_LIST_RESULT, ListResult);
	DDX_Control(pDX, IDC_EDIT_CONTENT, CEditContent);
	DDX_Control(pDX, IDC_PROGRESS1, CProgressCtrlStatus);
}

BEGIN_MESSAGE_MAP(CWeiboSpiderGUIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_BN_CLICKED(IDC_BUTTON1, &CWeiboSpiderGUIDlg::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BUTTON_GO, &CWeiboSpiderGUIDlg::OnBnClickedButtonGo)
ON_LBN_DBLCLK(IDC_LIST_RESULT, &CWeiboSpiderGUIDlg::OnLbnDblclkListResult)
END_MESSAGE_MAP()

BOOL CWeiboSpiderGUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	Pyenvinit = false;
	CProgressCtrlStatus.SetRange(0, 100);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWeiboSpiderGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CWeiboSpiderGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI ThreadFunc(LPVOID)
{
	// 初始化Python环境，引入Python模块
	if (false == Pyenvinit)
		PythonEnvInit();

	// 清空信息
	CProgressCtrlStatus.SetPos(0);
	CEditContent.Clear();
	ListResult.ResetContent();
	ResumeEnv();

	// 转换成ascii形式

	if (Url.GetLength() == 0)
	{
		MessageBoxA(NULL, "请输入Url", "Error", NULL);
		return 0;
	}
	USES_CONVERSION;
	char* WeiboUrl = W2A(Url.GetBuffer(0));

	CProgressCtrlStatus.SetPos(20);

	// 调用Python爬取微博正文信息
	CatchWeiboContent(WeiboUrl, CEditContent);
	CProgressCtrlStatus.SetPos(60);

	// 调用python爬取评论信息
	if (false == ChooseIndex)
	{
		CatchComments(WeiboUrl, ListResult);
		CProgressCtrlStatus.SetPos(100);
	}
	// 调用python爬取图片信息
	else
	{
		CatchPictures(WeiboUrl, ListResult);
		CProgressCtrlStatus.SetPos(100);
	}
	return 1;
}

void CWeiboSpiderGUIDlg::OnBnClickedButtonGo()
{
	if (NewThread != nullptr)
		CloseHandle(NewThread);
	UpdateData(TRUE);
	HANDLE NewThread = CreateThread(NULL, 0, ThreadFunc, 0, 0, NULL);
}

void CWeiboSpiderGUIDlg::OnLbnDblclkListResult()
{
	if (false != ChooseIndex)
		return;
	ExchangeLikePoints(ListResult.GetCurSel(), ListResult);
}
