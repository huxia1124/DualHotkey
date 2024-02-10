# DualHotkey
An implementation of Visual Studio dual hotkey

Visual Studio allows you to bind a combination of two hotkeys to a command, such as (Ctrl+K,Ctrl+C) to comment code and (Ctrl+K,Ctrl+U) to uncomment.

This is a sample of how the hotkey combination is achieved. It is an MFC application but can be easily ported to other Win32 projects.

How to use:
```
void CMainFrame::InitializeHotkeys()
{
  // Declare a member variable CDualHotkey m_dualHotkey;

	m_dualHotkey.SetCallback(this);

	// Setup hotkey Ctrl+M for command ID_FILE_NEW
	//////////////////////////////////////////////////////////////////////////
	// Ctrl + M
	ACCEL acc1;
	acc1.cmd = ID_FILE_NEW;
	acc1.fVirt = FCONTROL | FVIRTKEY;
	acc1.key = 'M';
	m_dualHotkey.AddAccelerator(acc1);

	// Setup hotkey Ctrl+K,Ctrl+A for command ID_APP_ABOUT
	//////////////////////////////////////////////////////////////////////////
	// Ctrl + K, Ctrl + A
	ACCEL acc2a;
	acc2a.cmd = ID_APP_ABOUT;	// or acc2a.cmd = 0. The cmd member of the 2nd ACCEL matters
	acc2a.fVirt = FCONTROL | FVIRTKEY;
	acc2a.key = 'K';

	ACCEL acc2b;
	acc2b.cmd = ID_APP_ABOUT;	// This command ID matters 
	acc2b.fVirt = FCONTROL | FVIRTKEY;
	acc2b.key = 'A';

	m_dualHotkey.AddAccelerator(acc2a, acc2b);

	// Setup hotkey Ctrl+K,Ctrl+P for command ID_FILE_PRINT
	//////////////////////////////////////////////////////////////////////////
	// Ctrl + K, Ctrl + P
	ACCEL acc3a;
	acc3a.cmd = ID_FILE_PRINT;		// or acc3a.cmd = 0. The cmd member of the 2nd ACCEL matters
	acc3a.fVirt = FCONTROL | FVIRTKEY;
	acc3a.key = 'K';

	ACCEL acc3b;
	acc3b.cmd = ID_FILE_PRINT;		// This command ID matters 
	acc3b.fVirt = FCONTROL | FVIRTKEY;
	acc3b.key = 'P';

	m_dualHotkey.AddAccelerator(acc3a, acc3b);
}
```

And call CDualHotkey::TranslateAccelerator to trigger the hotkey commands.
```
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (m_dualHotkey.TranslateAccelerator(GetSafeHwnd(), pMsg))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}
```
