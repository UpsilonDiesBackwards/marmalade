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
#include "components.h"

#include <windows.h>
#include <CommCtrl.h>
#include <shlobj.h>

#include <application/integration.h>

#include <zip.h>

#include <filesystem>
#include <iostream>
#include <unordered_map>

#define IDC_INSTALL_USER_CHK 1001
#define IDC_INSTALL_SYSTEM_CHK 1002
#define IDC_BACK_BTN 1003
#define IDC_NEXT_BTN 1004
#define IDC_HELP_BTN 1005
#define IDC_ABOUT_BTN 1006
#define IDC_COMPONENTS_LST 1008
#define IDC_FILE_ASSOC_CHK 1009
#define IDC_START_MENU_CHK 1011
#define IDC_DESKTOP_ICON_CHK 1012
#define IDC_QUICK_LAUNCH_CHK 1013

enum InstallMode {
    InstallMode_USER,
    InstallMode_SYSTEM,
    InstallMode_UNKNOWN
};

enum Step {
    Step_HOME = 0,
    Step_COMPONENTS = 1,
    Step_INTEGRATIONS = 2
};

struct State {
    InstallMode Mode;
    Step Step;
    std::vector<Component> Components;
    std::filesystem::path InstallPath;

    bool AddFileAssoc;
    bool AddStartMenuItem;
    bool AddDesktopItem;
    bool AddQuickLaunchItem;

    std::unordered_map<std::string, uint64_t> ComponentSizes;
};

State state;

bool bSilentMode = false;
bool bSystemInstall = false;

bool bAddingComponents = false;

HWND hLogo;
HWND hInstallCurrentUserChk;
HWND hInstallSystemChk;
HWND hHelpBtn;
HWND hAboutBtn;

HWND hInstallPathLbl;
HWND hInstallPathTxt;
HWND hComponentsLst;
HWND hEstimatedSpaceTxt;

HWND hFileAssocChk;
HWND hStartMenuChk;
HWND hDesktopIconChk;
HWND hQuickLaunchChk;

HWND hProgressBar;
HWND hBackBtn;
HWND hNextBtn;

HBRUSH hWhiteBrush = GetSysColorBrush(COLOR_WINDOW);

void CalculateSize() {
    int size = 0;
    for (const auto& com: state.Components) {
        if (com.Selected) {
            auto dirSize = state.ComponentSizes[com.DirName];
            size += dirSize;
        }
    }

    std::wstringstream ss;
    ss << "Estimated Size: " << size / (1024 * 1024) << " MB";
    SetWindowText(hEstimatedSpaceTxt, ss.str().c_str());
}

void UpdateUI() {
    SendMessage(hInstallCurrentUserChk, BM_SETCHECK, state.Mode == InstallMode_USER ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(hInstallSystemChk, BM_SETCHECK, state.Mode == InstallMode_SYSTEM ? BST_CHECKED : BST_UNCHECKED, 0);

    bAddingComponents = true;
    LVCOLUMN col = {0};
    col.mask = LVCF_WIDTH;
    col.cx = 280;
    ListView_InsertColumn(hComponentsLst, 0, &col);

    ListView_DeleteAllItems(hComponentsLst);
    for (int i = 0; i < state.Components.size(); ++i) {
        LVITEM item = {0};
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.pszText = const_cast<wchar_t*>(state.Components[i].DisplayName.c_str());
        ListView_InsertItem(hComponentsLst, &item);
        ListView_SetCheckState(hComponentsLst, i, state.Components[i].Selected);
    }
    CalculateSize();
    bAddingComponents = false;

    if (state.Mode == InstallMode_SYSTEM) {
        HICON hShield = LoadIcon(nullptr, IDI_SHIELD);
        HICON hShieldScaled = static_cast<HICON>(CopyImage(hShield, IMAGE_ICON, 16, 16, LR_COPYRETURNORG));
        SendMessage(hNextBtn, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(hShieldScaled));
    } else {
        SendMessage(hNextBtn, BM_SETIMAGE, IMAGE_ICON, 0);
    }

    SetWindowText(hInstallPathTxt, state.InstallPath.wstring().c_str());

    SendMessage(hFileAssocChk, BM_SETCHECK, state.AddFileAssoc ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(hStartMenuChk, BM_SETCHECK, state.AddStartMenuItem ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(hDesktopIconChk, BM_SETCHECK, state.AddDesktopItem ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(hQuickLaunchChk, BM_SETCHECK, state.AddQuickLaunchItem ? BST_CHECKED : BST_UNCHECKED, 0);
}

InstallMode GetInstallMode() {
    if (SendMessage(hInstallCurrentUserChk, BM_GETCHECK, 0, 0) == BST_CHECKED) {
        return InstallMode_USER;
    } else if (SendMessage(hInstallSystemChk, BM_GETCHECK, 0, 0) == BST_CHECKED) {
        return InstallMode_SYSTEM;
    }

    return InstallMode_UNKNOWN;
}

std::filesystem::path GetInstallPath(InstallMode mode) {
    PWSTR installPath = nullptr;

    if (mode == InstallMode_SYSTEM) {
        SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0, nullptr, &installPath);
    } else {
        SHGetKnownFolderPath(FOLDERID_UserProgramFiles, 0, nullptr, &installPath);
    }
    return std::filesystem::path(installPath) / "MarmaladeEngine";
}

void ShowStep(Step step) {
    EnableWindow(hBackBtn, step == Step_HOME ? FALSE : TRUE);
    SetWindowText(hNextBtn, step == Step_INTEGRATIONS ? L"Install" : L"Next");

    ShowWindow(hLogo, step == Step_HOME ? SW_SHOW : SW_HIDE);
    ShowWindow(hInstallCurrentUserChk, step == Step_HOME ? SW_SHOW : SW_HIDE);
    ShowWindow(hInstallSystemChk, step == Step_HOME ? SW_SHOW : SW_HIDE);

    ShowWindow(hInstallPathLbl, step == Step_COMPONENTS ? SW_SHOW : SW_HIDE);
    ShowWindow(hInstallPathTxt, step == Step_COMPONENTS ? SW_SHOW : SW_HIDE);
    ShowWindow(hComponentsLst, step == Step_COMPONENTS ? SW_SHOW : SW_HIDE);
    ShowWindow(hEstimatedSpaceTxt, step == Step_COMPONENTS ? SW_SHOW : SW_HIDE);

    ShowWindow(hFileAssocChk, step == Step_INTEGRATIONS ? SW_SHOW : SW_HIDE);
    ShowWindow(hStartMenuChk, step == Step_INTEGRATIONS ? SW_SHOW : SW_HIDE);
    ShowWindow(hDesktopIconChk, step == Step_INTEGRATIONS ? SW_SHOW : SW_HIDE);
    ShowWindow(hQuickLaunchChk, step == Step_INTEGRATIONS ? SW_SHOW : SW_HIDE);
}

std::string GetTemporaryFileName(const std::string& prefix) {
    char tempPath[MAX_PATH];
    char tempFile[MAX_PATH];

    DWORD pathLen = GetTempPathA(MAX_PATH, tempPath);
    if (pathLen == 0 || pathLen > MAX_PATH) {
        return "";
    }

    GUID guid;
    if (CoCreateGuid(&guid) != S_OK) {
        return "";
    }

    std::ostringstream oss;
    oss << tempPath << prefix << std::hex << std::setw(8) << std::setfill('0') << guid.Data1 << ".tmp";

    return oss.str();
}

bool ExtractZipFromResource(HINSTANCE hInstance, const char* outputPath) {
    HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(IDR_ZIPFILE), RT_RCDATA);
    if (!hRes) return false;

    HGLOBAL hResData = LoadResource(hInstance, hRes);
    if (!hResData) return false;

    void* pData = LockResource(hResData);
    DWORD size = SizeofResource(hInstance, hRes);

    HANDLE hFile = CreateFileA(outputPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    DWORD written;
    BOOL result = WriteFile(hFile, pData, size, &written, nullptr);
    CloseHandle(hFile);

    return result && written == size;
}

bool LoadZipResource(HINSTANCE hInstance, std::vector<char>& outBuffer) {
    HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(IDR_ZIPFILE), RT_RCDATA);
    if (!hRes) return false;

    HGLOBAL hData = LoadResource(hInstance, hRes);
    DWORD size = SizeofResource(hInstance, hRes);
    void* pData = LockResource(hData);
    if (!pData) return false;

    outBuffer.assign((char*) pData, (char*) pData + size);
    return true;
}

void CalculateZipSizes(zip_t* zip) {
    int n = zip_entries_total(zip);

    for (int i = 0; i < n; ++i) {
        zip_entry_openbyindex(zip, i);

        const char* name = zip_entry_name(zip);
        int isDir = zip_entry_isdir(zip);
        if (!isDir && name) {
            std::string path = name;

            size_t slash = path.find('/');
            if (slash != std::string::npos) {
                std::string topLevelDir = path.substr(0, slash);
                state.ComponentSizes[topLevelDir] += zip_entry_uncomp_size(zip);
            }
        }

        zip_entry_close(zip);
    }
}

bool RelaunchElevated() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(nullptr, exePath, MAX_PATH);

    auto components = ComponentsToParams(state.Components);

    SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.lpVerb = L"runas";
    sei.lpFile = exePath;

    std::wstringstream params;
    params << L"--system --components ";
    params << components;
    std::wstring paramsStr = params.str();
    sei.lpParameters = paramsStr.c_str();

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
    state.Components = {
            {L"Marmalade Engine", "_engine", true, true},
            {L"Engine Documentation", "_docs", false, true},
            {L"Plugin SDK", "_sdk", false, true},
            {L"Plugin Documentation", "_sdkdocs", false, true}};


    // Calculate sizes
    std::vector<char> zipData;
    if (LoadZipResource(hInstance, zipData)) {
        zip_t* zip = zip_stream_open(zipData.data(), zipData.size(), 0, 'r');
        if (!zip) return 0;

        CalculateZipSizes(zip);

        zip_close(zip);
    }


    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(pCmdLine, &argc);

    for (int i = 0; i < argc; i++) {
        if (wcscmp(argv[i], L"--silent") == 0) {
            bSilentMode = true;
        }
        if (wcscmp(argv[i], L"--system") == 0) {
            bSystemInstall = true;
        }
        if (wcscmp(argv[i], L"--components") == 0) {
            i++;
            ComponentsFromParams(argv[i], state.Components);
        }
    }

    if (bSilentMode) {
        std::cout << "Marmalade Installer" << std::endl;
        return 0;
    }

    if (bSystemInstall) {
        state.Mode = InstallMode_SYSTEM;
        state.Step = Step_COMPONENTS;
    }

    const wchar_t CLASS_NAME[] = L"mar";

    INITCOMMONCONTROLSEX icex = {};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_PROGRESS_CLASS | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    HICON hAppIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = hAppIcon;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    RegisterClassEx(&wc);

    HWND hWnd = CreateWindowEx(0, CLASS_NAME, L"Marmalade Installer", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
                               nullptr, nullptr, hInstance, nullptr);

    if (hWnd == nullptr) {
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

BOOL CALLBACK SetFontProc(HWND hWnd, LPARAM lParam) {
    SendMessage(hWnd, WM_SETFONT, lParam, TRUE);
    return TRUE;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORSTATIC: {
            return (INT_PTR) hWhiteBrush;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_CREATE: {
            BringWindowToTop(hWnd);

            // Step_HOME
            hLogo = CreateWindow(L"STATIC", L"",
                                 WS_CHILD | WS_VISIBLE | SS_ICON,
                                 170, 11, 192, 192,
                                 hWnd, nullptr, nullptr, nullptr);

            HANDLE hIcon = LoadImage(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, 0);
            SendMessage(hLogo, STM_SETICON, reinterpret_cast<WPARAM>(hIcon), 0);

            hInstallCurrentUserChk = CreateWindow(L"BUTTON", L"Install for Current User",
                                                  WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                                                  11, 11, 150, 17,
                                                  hWnd, (HMENU) IDC_INSTALL_USER_CHK, nullptr, nullptr);
            hInstallSystemChk = CreateWindow(L"BUTTON", L"Install for All Users",
                                             WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                                             11, 42, 150, 17,
                                             hWnd, (HMENU) IDC_INSTALL_SYSTEM_CHK, nullptr, nullptr);


            SendMessage(hInstallCurrentUserChk, BM_SETCHECK, BST_CHECKED, 0);

            // Step_COMPONENTS
            hInstallPathLbl = CreateWindow(L"STATIC", L"Install Path:", WS_VISIBLE | WS_CHILD,
                                           11, 11, 150, 13,
                                           hWnd, nullptr, nullptr, nullptr);

            hInstallPathTxt = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_TABSTOP,
                                             11, 29, 360, 23,
                                             hWnd, nullptr, nullptr, nullptr);

            hComponentsLst = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, L"",
                                            WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER | LVS_AUTOARRANGE,
                                            11, 59, 360, 104,
                                            hWnd, (HMENU) IDC_COMPONENTS_LST, nullptr, nullptr);

            hEstimatedSpaceTxt = CreateWindow(L"STATIC", L"Estimated Size: ", WS_VISIBLE | WS_CHILD,
                                              11, 170, 150, 13,
                                              hWnd, nullptr, nullptr, nullptr);

            state.InstallPath = GetInstallPath(state.Mode);

            ListView_SetExtendedListViewStyle(hComponentsLst, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);

            // Step_INTEGRATIONS
            hFileAssocChk = CreateWindow(L"BUTTON", L"File Associations",
                                         WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                         11, 11, 100, 17,
                                         hWnd, (HMENU) IDC_FILE_ASSOC_CHK, nullptr, nullptr);

            hStartMenuChk = CreateWindow(L"BUTTON", L"Start Menu Entries",
                                         WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                         11, 33, 125, 17,
                                         hWnd, (HMENU) IDC_START_MENU_CHK, nullptr, nullptr);

            hDesktopIconChk = CreateWindow(L"BUTTON", L"Desktop Icon",
                                           WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                           11, 55, 100, 17,
                                           hWnd, (HMENU) IDC_DESKTOP_ICON_CHK, nullptr, nullptr);

            hQuickLaunchChk = CreateWindow(L"BUTTON", L"Quick Launch Icon",
                                           WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                           11, 77, 125, 17,
                                           hWnd, (HMENU) IDC_QUICK_LAUNCH_CHK, nullptr, nullptr);

            // General
            hBackBtn = CreateWindow(L"BUTTON", L"Back",
                                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                    11, 190, 75, 23,
                                    hWnd, (HMENU) IDC_BACK_BTN, nullptr, nullptr);

            hNextBtn = CreateWindow(L"BUTTON", L"Next",
                                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                    93, 190, 75, 23,
                                    hWnd, (HMENU) IDC_NEXT_BTN, nullptr, nullptr);

            hHelpBtn = CreateWindow(L"BUTTON", L"Help",
                                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                    11, 220, 75, 23,
                                    hWnd, (HMENU) IDC_HELP_BTN, nullptr, nullptr);

            hAboutBtn = CreateWindow(L"BUTTON", L"About",
                                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                     93, 220, 75, 23,
                                     hWnd, (HMENU) IDC_ABOUT_BTN, nullptr, nullptr);

            hProgressBar = CreateWindow(PROGRESS_CLASS, L"", WS_VISIBLE | WS_CHILD,
                                        190, 220, 160, 15,
                                        hWnd, nullptr, nullptr, nullptr);

            ShowStep(state.Step);

            auto hFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
            EnumChildWindows(hWnd, SetFontProc, reinterpret_cast<LPARAM>(hFont));

            UpdateUI();

            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_ABOUT_BTN:
                case IDC_HELP_BTN:
                    ShellExecute(nullptr, L"open", L"https://github.com/Marmalade-Engine/marmalade", nullptr, nullptr, SW_SHOW);
                    break;
                case IDC_BACK_BTN: {
                    if (state.Step == Step_HOME) break;
                    state.Step = static_cast<Step>(state.Step - 1);
                    ShowStep(state.Step);
                    break;
                }
                case IDC_NEXT_BTN: {
                    if (state.Step == Step_INTEGRATIONS) {
                        // Perform install
                        CoInitialize(nullptr);

                        std::string tempZipName = GetTemporaryFileName("mar");
                        std::string tempDirName = GetTemporaryFileName("mar");
                        ExtractZipFromResource(GetModuleHandle(nullptr), tempZipName.c_str());

                        struct zip_t* zip = zip_open(tempZipName.c_str(), 0, 'r');
                        int i, n = zip_entries_total(zip);
                        for (i = 0; i < n; ++i) {
                            zip_entry_openbyindex(zip, i);
                            {
                                const char* name = zip_entry_name(zip);
                                int isdir = zip_entry_isdir(zip);
                                if (isdir) {
                                    std::filesystem::create_directories(std::filesystem::path(tempDirName) / name);
                                }
                            }
                            zip_entry_close(zip);
                        }
                        zip_close(zip);

                        int code = zip_extract(tempZipName.c_str(), tempDirName.c_str(), OnExtractEntry, nullptr);

                        std::vector<std::pair<CComPtr<IShellLinkW>, std::filesystem::path>> shortcutsToAdd{};

                        PWSTR startMenuPath = nullptr;
                        HRESULT hr = SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &startMenuPath);
                        if (FAILED(hr)) {
                            break;
                        }

                        PWSTR desktopPath = nullptr;
                        hr = SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &desktopPath);
                        if (FAILED(hr)) {
                            break;
                        }

                        PWSTR userPinnedPath = nullptr;
                        hr = SHGetKnownFolderPath(FOLDERID_UserPinned, 0, nullptr, &userPinnedPath);
                        if (FAILED(hr)) {
                            break;
                        }

                        std::filesystem::path smShortcutDir = std::filesystem::path(startMenuPath) / "Marmalade Engine";
                        std::filesystem::path desktopShortcutDir = std::filesystem::path(desktopPath);
                        std::filesystem::path upShortcutDir = std::filesystem::path(userPinnedPath) / "TaskBar";

                        // Install stuff
                        for (const auto& com: state.Components) {
                            if (!com.Selected) continue;

                            std::filesystem::path dstPath;
                            if (com.DirName == "_engine") {
                                dstPath = state.InstallPath;

                                CComPtr<IShellLinkW> shellLink;
                                hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
                                if (FAILED(hr)) {
                                    break;
                                }

                                shellLink->SetPath((state.InstallPath / "marmalade.exe").wstring().c_str());
                                shellLink->SetArguments(L"--project \"%1\"");
                                shellLink->SetIconLocation((state.InstallPath / "res/icons/logo/logo.ico").wstring().c_str(), 0);
                                shellLink->SetWorkingDirectory(state.InstallPath.wstring().c_str());

                                // Start menu shortcut already added by Marmalade integration API
                                if (state.AddDesktopItem) shortcutsToAdd.emplace_back(shellLink, desktopShortcutDir / "Marmalade Engine.lnk");
                                if (state.AddQuickLaunchItem) shortcutsToAdd.emplace_back(shellLink, upShortcutDir / "Marmalade Engine.lnk");

                            } else if (com.DirName == "_docs") {
                                dstPath = state.InstallPath / "EngineDocs";


                                CComPtr<IShellLinkW> shellLink;
                                hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
                                if (FAILED(hr)) {
                                    break;
                                }

                                shellLink->SetPath((dstPath / "html/index.html").wstring().c_str());

                                if (state.AddStartMenuItem) shortcutsToAdd.emplace_back(shellLink, smShortcutDir / "Engine Docs.lnk");
                                if (state.AddDesktopItem) shortcutsToAdd.emplace_back(shellLink, desktopShortcutDir / "Engine Docs.lnk");
                            } else if (com.DirName == "_sdk") {
                                dstPath = state.InstallPath / "PluginSDK";
                            } else if (com.DirName == "_sdkdocs") {
                                CComPtr<IShellLinkW> shellLink;
                                hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
                                if (FAILED(hr)) {
                                    break;
                                }
                                shellLink->SetPath((dstPath / "html/index.html").wstring().c_str());


                                if (state.AddStartMenuItem) shortcutsToAdd.emplace_back(shellLink, smShortcutDir / "Plugin SDK Docs.lnk");
                                if (state.AddDesktopItem) shortcutsToAdd.emplace_back(shellLink, desktopShortcutDir / "Plugin SDK Docs.lnk");

                                dstPath = state.InstallPath / "PluginSDKDocs";
                            }

                            std::filesystem::create_directories(dstPath);
                            std::filesystem::copy(std::filesystem::path(tempDirName) / com.DirName, dstPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::update_existing);
                        }

                        using it = Marmalade::Application::Integration::IntegrationType;
                        unsigned int type = it::IntegrationType_NONE;

                        if (state.AddFileAssoc)
                            type |= it::IntegrationType_FILE_ASSOCIATION;
                        if (state.AddStartMenuItem)
                            type |= it::IntegrationType_LAUNCHER;

                        Marmalade::Application::Integration::AddSystemIntegrations(static_cast<it>(type), state.InstallPath / "marmalade.exe");

                        // Start menu directories should already exist
                        std::filesystem::create_directories(smShortcutDir);

                        // Add shortcuts for other items
                        for (const auto& [shellLink, shortcutPath]: shortcutsToAdd) {
                            Marmalade::Application::Integration::AddShortcut(shellLink, shortcutPath);
                        }

                        CoTaskMemFree(startMenuPath);

                        // Delete temp files
                        std::filesystem::remove_all(tempZipName);
                        std::filesystem::remove_all(tempDirName);

                        break;
                    }

                    if (state.Step == Step_HOME) {
                        if (state.Mode == InstallMode_SYSTEM) {
                            if (RelaunchElevated()) {
                                PostQuitMessage(0);
                            }
                        }
                    }

                    state.Step = static_cast<Step>(state.Step + 1);
                    ShowStep(state.Step);
                    break;
                }
                case IDC_INSTALL_USER_CHK:
                case IDC_INSTALL_SYSTEM_CHK:
                    if (HIWORD(wParam) == BN_CLICKED) {
                        state.Mode = GetInstallMode();
                        state.InstallPath = GetInstallPath(state.Mode);
                        UpdateUI();
                    }
                    break;
                case IDC_FILE_ASSOC_CHK:
                    state.AddFileAssoc = SendMessage(hFileAssocChk, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    break;
                case IDC_START_MENU_CHK:
                    state.AddStartMenuItem = SendMessage(hStartMenuChk, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    break;
                case IDC_DESKTOP_ICON_CHK:
                    state.AddDesktopItem = SendMessage(hDesktopIconChk, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    break;
                case IDC_QUICK_LAUNCH_CHK:
                    state.AddQuickLaunchItem = SendMessage(hQuickLaunchChk, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    break;
            }
            break;
        case WM_NOTIFY: {
            auto hdr = (LPNMHDR) lParam;
            switch (hdr->idFrom) {
                case IDC_COMPONENTS_LST:
                    if (hdr->code == LVN_ITEMCHANGED) {
                        auto* nmv = (NMLISTVIEW*) lParam;

                        if ((nmv->uChanged & LVIF_STATE) && ((nmv->uNewState ^ nmv->uOldState) & INDEXTOSTATEIMAGEMASK(3))) {
                            if (bAddingComponents) break;// Ignore

                            int index = nmv->iItem;

                            if (state.Components[index].Required) {
                                state.Components[index].Selected = true;
                                ListView_SetCheckState(nmv->hdr.hwndFrom, index, BST_CHECKED);
                                break;
                            }

                            BOOL checked = ListView_GetCheckState(nmv->hdr.hwndFrom, index);
                            state.Components[index].Selected = checked;
                            CalculateSize();
                        }
                    } else if (hdr->code == LVN_GETINFOTIP) {
                        auto* infoTip = (NMLVGETINFOTIP*) lParam;

                        if (state.Components[infoTip->iItem].Required) {
                            wcscpy_s(infoTip->pszText, infoTip->cchTextMax, L"This component is required");
                        }
                    }
                    break;
            }
            break;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
