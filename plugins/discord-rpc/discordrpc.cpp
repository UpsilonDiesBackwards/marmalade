#include "discordrpc.h"

#include <discord_game_sdk.h>

#include <thread>
#include <interface.h>
#include <iostream>

struct IDiscordCore *core = nullptr;
struct DiscordActivity activity;

struct EngineAPI api;

void RunDiscordCallbacks() {
    while (true) {
        if (core) {
            core->run_callbacks(core);
        }

        auto name = api.InterfaceApi->GetFocusedWindow();
        strcpy_s(activity.details, sizeof(activity.details), name);
        struct IDiscordActivityManager *activityManager = core->get_activity_manager(core);
        activityManager->update_activity(activityManager, &activity, nullptr, nullptr);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void PluginMain(struct EngineAPI engineApi) {
    api = engineApi;
    DiscordCreateParams params{};
    DiscordCreateParamsSetDefault(&params);
    params.client_id = 1365764999611552054;
    params.flags = DiscordCreateFlags_Default;
    params.event_data = nullptr;

    enum EDiscordResult result = DiscordCreate(DISCORD_VERSION, &params, &core);
    if (result != DiscordResult_Ok) {
        LOG_ERROR(&engineApi, "Failed to create Discord Core! Error code: {}", result);
        return;
    }

    LOG_INFO(&engineApi, "Discord Core created");

    memset(&activity, 0, sizeof(activity));

    if (engineApi.IsDebuggerAttached()) {
        strcpy_s(activity.state, sizeof(activity.state), "Debugging Marmalade Engine...");
    } else if (engineApi.IsDebugMode()) {
        strcpy_s(activity.state, sizeof(activity.state), "Building Marmalade Engine...");
    } else {
        strcpy_s(activity.state, sizeof(activity.state), "Building a Game...");
    }

    strcpy_s(activity.details, sizeof(activity.details), "");

    struct IDiscordActivityManager *activityManager = core->get_activity_manager(core);
    activityManager->update_activity(activityManager, &activity, nullptr, nullptr);

    LOG_INFO(&engineApi, "Presence set");

    if (core) {
        std::thread discordCallbacksThread(RunDiscordCallbacks);
        discordCallbacksThread.detach();
    }
}

