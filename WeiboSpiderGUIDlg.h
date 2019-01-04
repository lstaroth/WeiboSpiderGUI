
// WeiboSpiderGUIDlg.h: 头文件
//

#pragma once


// CWeiboSpiderGUIDlg 对话框
class CWeiboSpiderGUIDlg : public CDialogEx
{
// 构造
public:
	CWeiboSpiderGUIDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WEIBOSPIDERGUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonGo();
	// 选择获取评论还是图片// 选择获取评论还是图片
	afx_msg void OnLbnDblclkListResult();
};
