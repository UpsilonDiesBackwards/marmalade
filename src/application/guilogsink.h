/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ENGINE_GUILOGSINK_H
#define ENGINE_GUILOGSINK_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>

#include <vector>

class GuiLogSink : public spdlog::sinks::base_sink<std::mutex> {
private:
    std::vector<std::pair<spdlog::level::level_enum, std::string>> items;
    std::mutex log_mutex;

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override;

    void flush_() override {};

public:
    const std::vector<std::pair<spdlog::level::level_enum, std::string>>& get_items();
};


#endif
