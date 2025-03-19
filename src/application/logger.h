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

#ifndef MARMALADE_LOGGER_H
#define MARMALADE_LOGGER_H

#include <spdlog/spdlog.h>

#ifdef DEBUG
#define LOG_IMPL(type, message, ...) spdlog::type("{}:{} " message, __FILE__, __LINE__ __VA_OPT__(, __VA_ARGS__))
#else
#define LOG_IMPL(type, message, ...) spdlog::type(message __VA_OPT__(, __VA_ARGS__))
#endif

#define LOG_TRACE(message, ...) LOG_IMPL(trace, message, __VA_ARGS__)
#define LOG_DEBUG(message, ...) LOG_IMPL(debug, message, __VA_ARGS__)
#define LOG_INFO(message, ...) LOG_IMPL(info, message, __VA_ARGS__)
#define LOG_WARN(message, ...) LOG_IMPL(warn, message, __VA_ARGS__)
#define LOG_ERROR(message, ...) LOG_IMPL(error, message, __VA_ARGS__)
#define LOG_CRITICAL(message, ...) LOG_IMPL(critical, message, __VA_ARGS__)

#endif
