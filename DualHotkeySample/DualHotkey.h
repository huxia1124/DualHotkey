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

class IDualHotkeyCallback
{
public:
	virtual void OnHotkeySkipped(ACCEL &accel) {};
	virtual void OnWaitingForHotkey(ACCEL &accel) {};
	virtual void OnHotkeyNotExists(ACCEL &acc1, ACCEL &acc2) {};
	virtual void OnHotkeyTranslated() {};
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

	DUAL_ACCEL* FindAcceleratorByCommand(UINT cmdId);

	const std::vector<DUAL_ACCEL> GetAllAccelerators() const { return _accList; }
	void SetCallback(IDualHotkeyCallback *pCallback) { _callback = pCallback; }
	IDualHotkeyCallback *GetCallback() const { return _callback; }
	void ImportAccelerators(HACCEL hAccel);

	// Return non-zero if translated, otherwise return 0
	int TranslateAccelerator(HWND hWnd, LPMSG lpMsg);

protected:
	DUAL_ACCEL* IsAcceleratorMatch(LPMSG lpMsg, DUAL_ACCEL &dacc);
	bool IsAcceleratorMatch(LPMSG lpMsg, ACCEL &accel);

	int TranslateSingleAccelerator(_In_ HWND hWnd, _In_ ACCEL &accel, _In_ LPMSG lpMsg);
	bool AcceleratorEqual(const ACCEL &acc1, const ACCEL &acc2);
	
protected:
	IDualHotkeyCallback *_callback;
	int _accIndex;
	DUAL_ACCEL *_lastDacc;
	std::vector<DUAL_ACCEL> _accList;
};

