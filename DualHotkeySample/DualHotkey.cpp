//
//Copyright(c) 2018. Huan Xia
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
//documentation files(the "Software"), to deal in the Software without restriction, including without limitation
//the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software,
//and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions
//of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
//TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL
//THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
//CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//DEALINGS IN THE SOFTWARE.

#include "stdafx.h"
#include "DualHotkey.h"

CDualHotkey::CDualHotkey()
	: _callback(nullptr)
	, _accIndex(0)
	, _lastDacc(nullptr)
{
}


CDualHotkey::~CDualHotkey()
{
}

bool CDualHotkey::AddAccelerator(const ACCEL &accel)
{
	ASSERT(accel.cmd != 0);

	for (DUAL_ACCEL &dacc : _accList)
	{
		if (AcceleratorEqual(dacc.acc1, accel))
		{
			return false;
		}
	}

	DUAL_ACCEL dacc;
	dacc.acc1 = accel;

	_accList.push_back(dacc);
	return true;
}

bool CDualHotkey::AddAccelerator(const ACCEL &acc1, const ACCEL &acc2)
{
	ASSERT(acc2.cmd != 0);
	ASSERT(acc1.cmd == acc2.cmd || acc1.cmd == 0);

	AddAccelerator(acc1);
	for (DUAL_ACCEL &dacc : _accList)
	{
		if (AcceleratorEqual(dacc.acc1, acc1))
		{
			for (ACCEL &acc : dacc.acc2)
			{
				if (AcceleratorEqual(acc, acc2))
				{
					return false;
				}
			}
			dacc.acc2.push_back(acc2);
			return true;
		}
	}

	return false;
}

CDualHotkey::DUAL_ACCEL* CDualHotkey::FindAcceleratorByCommand(UINT cmdId)
{
	for (DUAL_ACCEL &dacc : _accList)
	{
		if (dacc.acc2.empty())
		{
			if (dacc.acc1.cmd == cmdId)
			{
				return &dacc;
			}
		}
		else
		{
			for (ACCEL &acc : dacc.acc2)
			{
				if (acc.cmd == cmdId)
				{
					return &dacc;
				}
			}
		}
	}
	return nullptr;
}

void CDualHotkey::ImportAccelerators(HACCEL hAccel)
{
	std::vector<ACCEL> accList;
	int accelSize = ::CopyAcceleratorTable(hAccel, nullptr, 0);
	accList.resize(accelSize);
	::CopyAcceleratorTable(hAccel, accList.data(), accelSize);

	for (ACCEL &acc : accList)
	{
		AddAccelerator(acc);
	}
}

int CDualHotkey::TranslateAccelerator(HWND hWnd, LPMSG lpMsg)
{
	switch (lpMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_CHAR:
	case WM_SYSCHAR:
		break;

	default:
		return 0;
	}

	DUAL_ACCEL *pDacc = nullptr;
	if (_accIndex == 0)
	{
		for (DUAL_ACCEL &dacc : _accList)
		{
			pDacc = IsAcceleratorMatch(lpMsg, dacc);
			if (pDacc)
			{
				break;
			}
		}

		if (pDacc == nullptr)
		{
			if (_callback)
			{
				_callback->OnHotkeySkipped(pDacc->acc1);
			}

			return 0;
		}

		if (pDacc->acc2.empty())
		{
			return TranslateSingleAccelerator(hWnd, pDacc->acc1, lpMsg);
		}
		else
		{
			_lastDacc = pDacc;
			_accIndex = 1;

			if (_callback)
			{
				_callback->OnWaitingForHotkey(pDacc->acc1);
			}
		}
	}
	else if(lpMsg->message == WM_KEYDOWN)
	{
		switch (lpMsg->wParam)
		{
		case VK_CONTROL:
		case VK_SHIFT:
		case VK_MENU:
			return 0;
		}

		pDacc = _lastDacc;

		_accIndex = 0;
		_lastDacc = nullptr;

		for (ACCEL &acc : pDacc->acc2)
		{
			if (IsAcceleratorMatch(lpMsg, acc))
			{
				return TranslateSingleAccelerator(hWnd, acc, lpMsg);
			}
		}

		ACCEL accHit;
		accHit.fVirt = 0;
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			accHit.fVirt |= FCONTROL;
		}
		if (GetKeyState(VK_MENU) & 0x8000)
		{
			accHit.fVirt |= FALT;
		}
		if (GetKeyState(VK_SHIFT) & 0x8000)
		{
			accHit.fVirt |= FSHIFT;
		}
		accHit.key = LOWORD(lpMsg->wParam);
		accHit.fVirt |= FVIRTKEY;
		if (_callback)
		{
			_callback->OnHotkeyNotExists(pDacc->acc1, accHit);
		}

		return 1;
	}

	return 0;
}

CDualHotkey::DUAL_ACCEL *CDualHotkey::IsAcceleratorMatch(LPMSG lpMsg, DUAL_ACCEL &dacc)
{
	return IsAcceleratorMatch(lpMsg, dacc.acc1) ? &dacc : nullptr;
}

bool CDualHotkey::IsAcceleratorMatch(LPMSG lpMsg, ACCEL &accel)
{
	BOOL fVirt = FALSE;

	switch (lpMsg->message) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		fVirt = TRUE;
		break;

	case WM_CHAR:
	case WM_SYSCHAR:
		fVirt = FALSE;
		break;

	default:
		return false;
	}

	UINT keystate = 0;
	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		keystate |= FCONTROL;
	}
	if (GetKeyState(VK_MENU) & 0x8000)
	{
		keystate |= FALT;
	}
	if (GetKeyState(VK_SHIFT) & 0x8000)
	{
		keystate |= FSHIFT;
	}

	UINT flags = accel.fVirt;
	if ((DWORD)accel.key != lpMsg->wParam ||
		((fVirt != 0) != ((flags & FVIRTKEY) != 0)))
	{
		return false;
	}

	if (fVirt && ((keystate & (FSHIFT | FCONTROL)) != (flags & (FSHIFT | FCONTROL))))
	{
		return false;
	}

	if ((keystate & FALT) != (flags & FALT))
	{
		return false;
	}

	return true;
}

int CDualHotkey::TranslateSingleAccelerator(_In_ HWND hWnd, _In_ ACCEL &accel, _In_ LPMSG lpMsg)
{
	if (_callback)
	{
		_callback->OnHotkeyTranslated();
	}

	HACCEL hAccel = CreateAcceleratorTable(&accel, 1);
	int result = ::TranslateAccelerator(hWnd, hAccel, lpMsg);
	DestroyAcceleratorTable(hAccel);
	return result;
}

bool CDualHotkey::AcceleratorEqual(const ACCEL &acc1, const ACCEL &acc2)
{
	return acc1.fVirt == acc2.fVirt && acc1.key == acc2.key;
}
