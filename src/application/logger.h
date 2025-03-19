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

#if defined(__cpp_va_opt) || !defined(_MSC_VER)

#define LOG_IMPL(type, message, ...) spdlog::type("{}:{} " message, __FILE__, __LINE__ __VA_OPT__(, __VA_ARGS__))

#else

#define LOG_IMPL(type, message, ...) spdlog::type("{}:{} " message, __FILE__, __LINE__, __VA_ARGS__)

#endif // __cpp_va_opt

#else

#if defined(__cpp_va_opt) || !defined(_MSC_VER)

#define LOG_IMPL(type, message, ...) spdlog::type(message __VA_OPT__(, __VA_ARGS__))

#else

#define LOG_IMPL(type, message, ...) spdlog::type(message, __VA_ARGS__)

#endif // __cpp_va_opt

#endif // DEBUG

#define LOG_TRACE(...) LOG_IMPL(trace, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_IMPL(debug, __VA_ARGS__)
#define LOG_INFO(...) LOG_IMPL(info, __VA_ARGS__)
#define LOG_WARN(...) LOG_IMPL(warn, __VA_ARGS__)
#define LOG_ERROR(...) LOG_IMPL(error, __VA_ARGS__)
#define LOG_CRITICAL(...) LOG_IMPL(critical, __VA_ARGS__)

#endif
