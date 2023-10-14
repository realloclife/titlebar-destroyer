#include <iostream>
#include <system_error>
#include <thread>
#include <Windows.h>

auto remove_titlebar(HWND handle, LPARAM) noexcept -> BOOL CALLBACK {
	auto style = GetWindowLong(handle, GWL_STYLE);
	style &= ~WS_CAPTION;
	SetWindowLong(handle, GWL_STYLE, style);
	auto window = RECT{};
	GetWindowRect(handle, &window);
	SetWindowPos(handle, nullptr,
		window.left, window.top,
		window.right - window.left,
		window.bottom - window.top,
		SWP_FRAMECHANGED);
	return TRUE;
}

auto new_window_hook(HWINEVENTHOOK, DWORD event, HWND, LONG, LONG, DWORD, DWORD) noexcept -> void CALLBACK {
	if (event == EVENT_OBJECT_CREATE) {
		EnumWindows(remove_titlebar, 0); // currently unoptimized, there's better ways to do this
	}
}

auto main() -> int {
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	EnumWindows(remove_titlebar, 0);
	auto event_hook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, nullptr, new_window_hook, 0, 0, WINEVENT_OUTOFCONTEXT);
	if (!event_hook) {
		auto error_id = GetLastError();
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		std::cerr << "Couldn't set up event hook\n";
		std::cerr << "GetLastError: " << std::system_category().message(error_id) << '\n';
		std::this_thread::sleep_for(std::chrono::milliseconds{ 3000 });
		return EXIT_FAILURE;
	}
	auto message = MSG{};
	while (GetMessage(&message, nullptr, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return EXIT_SUCCESS;
}