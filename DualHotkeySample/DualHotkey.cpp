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
	return InternalAddAccelerator(accel);
}

bool CDualHotkey::AddAccelerator(const ACCEL &acc1, const ACCEL &acc2)
{
	ASSERT(acc2.cmd != 0);
	ASSERT(acc1.cmd == acc2.cmd || acc1.cmd == 0);

	InternalAddAccelerator(acc1);
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

const CDualHotkey::DUAL_ACCEL* CDualHotkey::FindAcceleratorByCommand(UINT cmdId) const
{
	for (const DUAL_ACCEL &dacc : _accList)
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
			for (const ACCEL &acc : dacc.acc2)
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

	for (const ACCEL &acc : accList)
	{
		AddAccelerator(acc);
	}
}

int CDualHotkey::TranslateAccelerator(HWND hWnd, LPMSG lpMsg)
{
	switch (lpMsg->message)
	{
		// Only cares about these 4 message types
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_CHAR:
	case WM_SYSCHAR:
		break;

	default:
		return 0;
	}

	const DUAL_ACCEL *pDacc = nullptr;
	if (_accIndex == 0)
	{
		// Current index is 0, we are looking for the first part of the accelerator pair
		// Find the accelerator pair based on the first accelerator of the pair
		for (const DUAL_ACCEL &dacc : _accList)
		{
			pDacc = GetDualAccelerator(lpMsg, dacc);
			if (pDacc)
			{
				break;
			}
		}

		if (pDacc == nullptr)
		{
			// First accelerator is not found.
			if (_callback)
			{
				_callback->OnHotkeySkipped(pDacc->acc1);
			}

			return 0;
		}

		// Found first accelerator
		if (pDacc->acc2.empty())
		{
			// But this accelerator pair is cinfigured to be single hotkey. Translate and trigger it immediately.
			return TranslateSingleAccelerator(hWnd, pDacc->acc1, nullptr, lpMsg);
		}
		else
		{
			// Set up member variables  to get ready for the next hotkey
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
		// If this is a virtua-keys-only message (i.e. pressed Ctrl and released without actually triggering a hotkey combination like Ctrl+C)
		switch (lpMsg->wParam)
		{
		case VK_CONTROL:
		case VK_SHIFT:
		case VK_MENU:
			return 0;
		}

		pDacc = _lastDacc;
		ASSERT(pDacc);

		// No matter whether a command is found, just reset the member variables so we can start waiting for a new accelerator pair
		_accIndex = 0;
		_lastDacc = nullptr;

		// find the matched 2nd accelerator. If found, trigger the command.
		for (const ACCEL &acc : pDacc->acc2)
		{
			if (IsAcceleratorMatch(lpMsg, acc))
			{
				return TranslateSingleAccelerator(hWnd, pDacc->acc1, &acc, lpMsg);
			}
		}

		// If not found, report a hotkey miss through callback
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

bool CDualHotkey::InternalAddAccelerator(const ACCEL& accel)
{
	for (DUAL_ACCEL& dacc : _accList)
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

const CDualHotkey::DUAL_ACCEL* CDualHotkey::GetDualAccelerator(LPMSG lpMsg, const DUAL_ACCEL &dacc) const
{
	return IsAcceleratorMatch(lpMsg, dacc.acc1) ? &dacc : nullptr;
}

bool CDualHotkey::IsAcceleratorMatch(LPMSG lpMsg, const ACCEL &accel) const
{
	BOOL fVirt = FALSE;

	switch (lpMsg->message)
	{
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

	// The 'key' does not match
	if (static_cast<DWORD>(accel.key) != lpMsg->wParam)
		return false;

	// Check virtual keys like Ctrl, ALT, Shift etc.
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
	if (((fVirt != 0) != ((flags & FVIRTKEY) != 0)))
	{
		// virtual keys do not match
		return false;
	}

	if (fVirt && ((keystate & (FSHIFT | FCONTROL)) != (flags & (FSHIFT | FCONTROL))))
	{
		// virtual keys do not match for Shift+Ctrl combination
		return false;
	}

	if ((keystate & FALT) != (flags & FALT))
	{
		// virtual keys do not match for ALT
		return false;
	}

	return true;
}

int CDualHotkey::TranslateSingleAccelerator(HWND hWnd, const ACCEL &acc1, const ACCEL* acc2, LPMSG lpMsg)
{
	const ACCEL* accEffective = acc2 ? acc2 : &acc1;

	if (_callback)
	{
		if (!_callback->IsHotkeyEnabled(*accEffective))
		{
			_callback->OnHotkeyDenied(acc1, acc2);
			return 1;
		}

		_callback->OnHotkeyTranslated(*accEffective);
	}

	ACCEL accelCopy = *accEffective;
	HACCEL hAccel = CreateAcceleratorTable(&accelCopy, 1);
	int result = ::TranslateAccelerator(hWnd, hAccel, lpMsg);
	DestroyAcceleratorTable(hAccel);
	return result;
}

bool CDualHotkey::AcceleratorEqual(const ACCEL &acc1, const ACCEL &acc2)
{
	return acc1.fVirt == acc2.fVirt && acc1.key == acc2.key;
}
