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

#pragma once

#include <vector>

// Callback functions
class IDualHotkeyCallback
{
public:
	virtual void OnHotkeySkipped(const ACCEL &accel) {}
	virtual void OnHotkeyDenied(const ACCEL& acc1, const ACCEL* acc2) {}
	virtual void OnWaitingForHotkey(const ACCEL &accel) {}
	virtual void OnHotkeyNotExists(const ACCEL &acc1, const ACCEL &acc2) {}
	virtual void OnHotkeyTranslated(const ACCEL& accel) {}
	virtual bool IsHotkeyEnabled(const ACCEL& accel) { return true; }
};

class CDualHotkey
{
public:
	CDualHotkey();
	virtual ~CDualHotkey();

	struct DUAL_ACCEL
	{
		ACCEL acc1;
		std::vector<ACCEL> acc2;
	};

	bool AddAccelerator(const ACCEL &accel);
	bool AddAccelerator(const ACCEL &acc1, const ACCEL &acc2);

	const DUAL_ACCEL* FindAcceleratorByCommand(UINT cmdId) const;

	const std::vector<DUAL_ACCEL>& GetAllAccelerators() const { return _accList; }

	void SetCallback(IDualHotkeyCallback *pCallback) { _callback = pCallback; }
	IDualHotkeyCallback *GetCallback() const { return _callback; }

	// Import accelerator configurations from a given accelerator table.
	void ImportAccelerators(HACCEL hAccel);

	// Call this function in your message loop to monitor keyboard events and trigger commands
	// Returns non-zero if translated, otherwise return 0
	int TranslateAccelerator(HWND hWnd, LPMSG lpMsg);

protected:
	bool InternalAddAccelerator(const ACCEL& accel);

	// Check to see if the given message matches the first accelerator of the given accelerator pair
	// if it matches, returns the pointer of the accelerator pair (the address of the input dacc parameter)
	// returns nullptr if it does not match.
	const DUAL_ACCEL* GetDualAccelerator(LPMSG lpMsg, const DUAL_ACCEL &dacc) const;

	// Check to see if the given message matches the accelerator configuration
	bool IsAcceleratorMatch(LPMSG lpMsg, const ACCEL &accel) const;

	int TranslateSingleAccelerator(HWND hWnd, const ACCEL& acc1, const ACCEL* acc2, LPMSG lpMsg);

	// Check to see if the two given accelerators are equal (same keys and same virtual keys)
	static bool AcceleratorEqual(const ACCEL &acc1, const ACCEL &acc2);
	
protected:

	// Callback
	IDualHotkeyCallback *_callback;

	// Current accelerator index, can be 0 or 1
	int _accIndex;

	// When dual accelerators 
	const DUAL_ACCEL *_lastDacc;

	// Dual hotkey configurations
	std::vector<DUAL_ACCEL> _accList;
};

