#include "guilogsink.h"

void GuiLogSink::sink_it_(const spdlog::details::log_msg& msg) {
    spdlog::memory_buf_t formatted;
    formatter_->format(msg, formatted);

    std::lock_guard<std::mutex> lock(log_mutex);
    items.emplace_back(msg.level, fmt::to_string(formatted));
}

const std::vector<std::pair<spdlog::level::level_enum, std::string>>& GuiLogSink::get_items() {
    std::lock_guard<std::mutex> lock(log_mutex);
    return items;
}
