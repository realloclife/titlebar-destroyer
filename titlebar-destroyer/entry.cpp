#include <windows.h>
#include <UIAutomation.h>

#pragma comment(lib, "UIAutomationCore.lib")

auto win_event_proc(HWINEVENTHOOK, DWORD, HWND window, LONG object_id, LONG child_id, DWORD, DWORD) -> void CALLBACK {
	if (object_id == OBJID_WINDOW && child_id == CHILDID_SELF) {
		IUIAutomationElement* pElement{};
		IUIAutomation* pAutomation{};
		if (FAILED(CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_IUIAutomation, reinterpret_cast<void**>(&pAutomation)))) {
			return;
		}
		pAutomation->ElementFromHandle(window, &pElement);
		SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_CAPTION);
		RedrawWindow(window, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
		if (!pElement) {
			return;
		}
		pElement->Release();
		pAutomation->Release();
	}
}

auto enum_windows_proc(HWND window, LPARAM) -> BOOL CALLBACK {
	SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_CAPTION);
		RedrawWindow(window, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
	return TRUE;
}

auto WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) -> int WINAPI {
	if (FAILED(CoInitialize(nullptr))) {
		return EXIT_FAILURE;
	}
	auto hook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, nullptr, win_event_proc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
	EnumWindows(enum_windows_proc, 0);
	auto msg = MSG{};
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnhookWinEvent(hook);
	CoUninitialize();
	return EXIT_SUCCESS;
}