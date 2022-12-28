
// InjectDlgDlg.h: 头文件
//

#pragma once


// CInjectDlgDlg 对话框
class CInjectDlgDlg : public CDialogEx
{
// 构造
public:
	CInjectDlgDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INJECTDLG_DIALOG };
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
	CEdit m_dllname;
	CEdit m_info;
	std::vector<std::string> m_processNameVec;
	afx_msg void OnBnClickedButtonInject();
	void UnInjectDll();
	afx_msg void OnClose();
	CComboBox m_cbProcessName;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CButton m_btnInject;
};
