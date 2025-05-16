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

#ifndef MARMALADE_PROJECTBROWSER_API_H
#define MARMALADE_PROJECTBROWSER_API_H

#define MATCH_BY_EXTENSION(ext)                                                 \
    .MatchFunc = engineApi.ProjectBrowserApi->GetRegistry()->MatchByExtension, \
    .MatchFuncArgs = &(struct MatchByExtArgs) { ext }

#define MATCH_BY_SIGNATURE(sig, sigLen)                                         \
    .MatchFunc = engineApi.ProjectBrowserApi->GetRegistry()->MatchBySignature, \
    .MatchFuncArgs = &(struct MatchBySignatureArgs) { sig, sigLen }

struct MatchByExtArgs {
    char* Extension;
};

struct MatchBySignatureArgs {
    char* Signature;
    int SignatureLen;
};

struct ENGINE_PROVIDED ProjectBrowserFunctionRegistry {
    MarmResult (*MatchByExtension)(void* args);
    MarmResult (*MatchBySignature)(void* args);
    int (*DefaultRename)(void* item);
    // ... other project browser functions
};

struct ProjectBrowserAPI {
    void (*Init)();
    struct ProjectBrowserFunctionRegistry* (*GetRegistry)();
};

struct FileTypeCategory {
    char* Name;
    char* Description;
};

struct PLUGIN_PROVIDED FileRegistration {
    char* Name;
    char* Description;

    MarmResult (*MatchFunc)(void *args);
    void* MatchFuncArgs;

    char* (*DisplayNameFunc)();

    MarmResult (*LoadThumbnailFunc)();

    MarmResult (*ContextMenuItemsFunc)();
    MarmResult (*ItemRenameFunc)();
    MarmResult (*ItemDeleteFunc)();
    MarmResult (*ItemClickFunc)();

    void (*FreeItemFunc)();

    struct FileTypeCategory** categories;
};

#endif
