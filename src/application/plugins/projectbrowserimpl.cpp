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

#include "projectbrowserimpl.h"

#include "../logger.h"

MarmResult Marmalade::ProjectBrowserApiImpl::MatchByExtension(void* args) {
    LOG_INFO("MatchByExtension");
    return MARM_RESULT_FAILURE;
}

MarmResult Marmalade::ProjectBrowserApiImpl::MatchBySignature(void* args) {
    LOG_INFO("MatchBySignature");
    return MARM_RESULT_FAILURE;
}

int Marmalade::ProjectBrowserApiImpl::DefaultRename(void* item) {
    LOG_INFO("DefaultRename");
    return false;
}

void Marmalade::ProjectBrowserApiImpl::Init() {
    LOG_INFO("Project browser init");
}

struct ProjectBrowserFunctionRegistry* Marmalade::ProjectBrowserApiImpl::GetRegistry() {
    static auto funcs = ProjectBrowserFunctionRegistry{
            .MatchByExtension = &ProjectBrowserApiImpl::MatchByExtension,
            .DefaultRename = &ProjectBrowserApiImpl::DefaultRename};
    return &funcs;
}
