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

#include "integration.h"

#ifndef DISABLE_LOGGING
#include "logger.h"
#endif

#include <filesystem>

#ifdef WIN32
#include <shlobj.h>
#include <shobjidl.h>
#include <objbase.h>
#include <shlguid.h>
#include <atlcomcli.h>

bool Marmalade::Application::Integration::SetRegistryValue(HKEY root, const std::wstring& subkey, const std::wstring& valName, const std::wstring& val) {
    HKEY key;
    if (RegCreateKeyExW(root, subkey.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &key, nullptr) != ERROR_SUCCESS) {
#ifndef DISABLE_LOGGING
        LOG_ERROR("RegCreateKeyExA failed");
#endif
        return false;
    }

    LONG res = RegSetValueExW(key, valName.c_str(), 0, REG_SZ,
                              reinterpret_cast<const BYTE*>(val.c_str()),
                              (val.size() + 1) * sizeof(wchar_t));

    RegCloseKey(key);
    if (res != ERROR_SUCCESS) {
#ifndef DISABLE_LOGGING
        LOG_ERROR("RegSetValueExA failed: {}", res);
#endif
        return false;
    }
    return true;
}
#endif

void Marmalade::Application::Integration::AddSystemIntegrations(IntegrationType types) {
#ifndef DISABLE_LOGGING
    LOG_INFO("Adding system integrations");
#endif

#ifdef WIN32
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();
    SetCurrentDirectoryW(exeDir.c_str());
#endif

    if (types & IntegrationType_FILE_ASSOCIATION) {
#ifdef WIN32
        const wchar_t* ext = L".marmalade";
        const wchar_t* progId = L"Marmalade.Project";
        const wchar_t* description = L"Marmalade Project";

        SetRegistryValue(HKEY_CURRENT_USER, std::wstring(L"Software\\Classes\\") + ext, L"", progId);
        SetRegistryValue(HKEY_CURRENT_USER, std::wstring(L"Software\\Classes\\") + progId, L"", description);
        SetRegistryValue(HKEY_CURRENT_USER, std::wstring(L"Software\\Classes\\") + progId + L"\\DefaultIcon", L"", std::wstring(exePath) + L",0");
        SetRegistryValue(HKEY_CURRENT_USER, std::wstring(L"Software\\Classes\\") + progId + L"\\shell\\open\\command", L"", std::wstring(L"\"") + exePath + L"\" --project \"%1\"");

        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
#endif
    }

    if (types & IntegrationType_LAUNCHER) {
#ifdef WIN32
        CoInitialize(nullptr);

        CComPtr<IShellLinkW> shellLink;
        HRESULT hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
        if (FAILED(hr)) {
#ifndef DISABLE_LOGGING
            LOG_ERROR("Failed to create shell link instance: {}", hr);
#endif
            return;
        }

        shellLink->SetPath(exePath);
        shellLink->SetArguments(L"--project \"%1\"");
        shellLink->SetIconLocation((exeDir / "res/icons/logo/logo.ico").wstring().c_str(), 0);
        shellLink->SetWorkingDirectory(exeDir.wstring().c_str());

        PWSTR startMenuPath = nullptr;
        hr = SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &startMenuPath);
        if (FAILED(hr)) {
#ifndef DISABLE_LOGGING
            LOG_ERROR("Failed to find programs folder: {}", hr);
#endif
            return;
        }

        std::filesystem::path shortcutDir = std::filesystem::path(startMenuPath) / "Marmalade Engine";
        std::filesystem::create_directories(shortcutDir);

        std::filesystem::path shortcutPath = shortcutDir / "Marmalade Engine.lnk";
        CoTaskMemFree(startMenuPath);

        CComPtr<IPersistFile> persistFile;
        hr = shellLink->QueryInterface(IID_PPV_ARGS(&persistFile));
        if (FAILED(hr)) {
#ifndef DISABLE_LOGGING
            LOG_ERROR("Failed to create file instance: {}", hr);
#endif
            return;
        }

        hr = persistFile->Save(shortcutPath.wstring().c_str(), TRUE);
        if (FAILED(hr)) {
#ifndef DISABLE_LOGGING
            LOG_ERROR("Failed to create shortcut: {}", hr);
#endif
            return;
        }
#endif
    }
}

void Marmalade::Application::Integration::MarkRecentFile(const std::filesystem::path& path) {
#ifdef WIN32
    SHAddToRecentDocs(SHARD_PATHA, absolute(path).string().c_str());
#endif
}
