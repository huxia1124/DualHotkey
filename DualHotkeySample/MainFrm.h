
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "DualHotkey.h"

class CMainFrame : public CMDIFrameWndEx, public IDualHotkeyCallback
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CFileView         m_wndFileView;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;

	CDualHotkey       m_dualHotkey;

protected:
	CString GetAcceleratorString(ACCEL& accel)
	{
		CMFCAcceleratorKey key(&accel);
		CString hotkeyText;
		key.Format(hotkeyText);
		return hotkeyText;
	}

	void OnWaitingForHotkey(ACCEL& accel)
	{
		CMFCStatusBar* p = (CMFCStatusBar*)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
		CString sKey1 = GetAcceleratorString(accel);
		CString sMsg = _T("Waiting for hotkey: ") + sKey1 + _T(" + ?");
		p->SetPaneText(0, sMsg);
	}

	void OnHotkeyNotExists(ACCEL& acc1, ACCEL& acc2)
	{
		CMFCStatusBar* p = (CMFCStatusBar*)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
		CString sKey1 = GetAcceleratorString(acc1);
		CString sKey2 = GetAcceleratorString(acc2);
		CString sMsg = _T("The key combination (") + sKey1 + _T(",") + sKey2 + _T(") is not a command!");

		p->SetPaneText(0, sMsg);
	}

	void OnHotkeyTranslated()
	{
		CMFCStatusBar* p = (CMFCStatusBar*)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
		p->SetPaneText(0, _T("Ready"));
	}

	void OnHotkeySkipped(ACCEL& accel)
	{
		//CMFCStatusBar* p = (CMFCStatusBar*)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
		//p->SetPaneText(0, _T("Ready"));
	}


	void InitializeHotkeys();

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


