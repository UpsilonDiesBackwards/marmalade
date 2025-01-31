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
