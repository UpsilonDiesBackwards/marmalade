/*
 Marmalade - Lightweight Game Engine
 Copyright (C) 2025 Tayler Parsons
 Copyright (C) 2025 Ryan Bester

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef UNICODE
#define UNICODE
#endif

#include "resource.h"

#include <windows.h>

#include <application/integration.h>

#include <zip.h>

#include <filesystem>
#include <locale>
#include <codecvt>

#define IDC_BUTTON_INSTALL_USER 1001
#define IDC_BUTTON_INSTALL_SYSTEM 1002

bool bSilentMode = false;
bool bElevatedMode = false;

HWND hInstallAllUsers;

std::wstring GetTemporaryFileName(const std::wstring& prefix) {
    wchar_t tempPath[MAX_PATH];
    wchar_t tempFile[MAX_PATH];

    DWORD pathLen = GetTempPath(MAX_PATH, tempPath);
    if (pathLen == 0 || pathLen > MAX_PATH) {
        return L"";
    }

    if (GetTempFileName(tempPath, prefix.c_str(), 0, tempFile) == 0) {
        return L"";
    }

    return tempFile;
}

bool ExtractZipFromResource(HINSTANCE hInstance, const wchar_t* outputPath) {
    HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(IDR_ZIPFILE), RT_RCDATA);
    if (!hRes) return false;

    HGLOBAL hResData = LoadResource(hInstance, hRes);
    if (!hResData) return false;

    void* pData = LockResource(hResData);
    DWORD size = SizeofResource(hInstance, hRes);

    HANDLE hFile = CreateFileW(outputPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    DWORD written;
    BOOL result = WriteFile(hFile, pData, size, &written, nullptr);
    CloseHandle(hFile);

    return result && written == size;
}

bool RelaunchElevated() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(nullptr, exePath, MAX_PATH);

    SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.lpVerb = L"runas";
    sei.lpFile = exePath;
    sei.lpParameters = L"--elevated";
    sei.nShow = SW_NORMAL;

    if (!ShellExecuteEx(&sei)) {
        DWORD err = GetLastError();
        if (err == ERROR_CANCELLED) {
            MessageBox(nullptr, L"User declined elevation", L"Info", MB_OK);
        } else {
            MessageBox(nullptr, L"Failed to elevate", L"Error", MB_OK);
        }
        return false;
    }

    return true;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int OnExtractEntry(const char* filename, void* arg) {
    static int i = 0;
    printf("Extracted: %s (%d)\n", filename, ++i);

    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(pCmdLine, &argc);

    for (int i = 0; i < argc; i++) {
        if (wcscmp(argv[i], L"--silent") == 0) {
            bSilentMode = true;
        }
        if (wcscmp(argv[i], L"--elevated") == 0) {
            bElevatedMode = true;
        }
    }

    const wchar_t CLASS_NAME[] = L"mar";

    HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = hIcon;

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(0, CLASS_NAME, L"Marmalade Installer", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 300, 300,
                               nullptr, nullptr, hInstance, nullptr);

    if (hWnd == nullptr) {
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);

    std::wstring tempName = GetTemporaryFileName(L"mar");
    ExtractZipFromResource(hInstance, tempName.c_str());

    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    std::string converted_str = converter.to_bytes(tempName);

    struct zip_t* zip = zip_open(converted_str.c_str(), 0, 'r');
    int i, n = zip_entries_total(zip);
    for (i = 0; i < n; ++i) {
        zip_entry_openbyindex(zip, i);
        {
            const char* name = zip_entry_name(zip);
            int isdir = zip_entry_isdir(zip);
            if (isdir) {
                std::filesystem::create_directories(std::filesystem::path("extracted.dir") / name);
            }
        }
        zip_entry_close(zip);
    }
    zip_close(zip);

    int code = zip_extract(converted_str.c_str(), "extracted.dir", OnExtractEntry, nullptr);
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_CREATE: {
            BringWindowToTop(hWnd);

            CreateWindowW(L"BUTTON", L"Install for Current User",
                          WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                          10, 10, 150, 30,
                          hWnd, (HMENU) IDC_BUTTON_INSTALL_USER, nullptr, nullptr);
            hInstallAllUsers = CreateWindowW(L"BUTTON", L"Install for All Users",
                                             WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                             170, 10, 150, 30,
                                             hWnd, (HMENU) IDC_BUTTON_INSTALL_SYSTEM, nullptr, nullptr);
            HICON shield = LoadIcon(nullptr, IDI_SHIELD);
            SendMessage(hInstallAllUsers, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(shield));

            CreateWindowW(L"BUTTON", L"More Options",
                          WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
                          10, 60, 100, 30,
                          hWnd, nullptr, nullptr, nullptr);

            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));

            EndPaint(hWnd, &ps);
        }
            return 0;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_BUTTON_INSTALL_USER:
                    MessageBox(hWnd, L"Installing Marmalade Engine", L"Install", MB_OK);
                    break;
                case IDC_BUTTON_INSTALL_SYSTEM:
                    if (RelaunchElevated()) {
                        PostQuitMessage(0);
                    }
                    break;
            }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
