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

#include "audioplayer.h"

#include "../../../application/config/engineconfig.h"

#include <imgui.h>
#include <ImGuiFileDialog.h>

#include <IconsCodicons.h>

void Marmalade::ECS::AudioPlayer::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    if (_audioFilePath.empty()) {
        if (ImGui::Button(ICON_CI_FOLDER_OPENED " Load Audio File")) {
            IGFD::FileDialogConfig config;
            config.path =
                    EngineConfig::GetStoredConfig().defaultProjectPath;
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("LoadAudio", "Load Audio File", ".wav", config);
        }

        if (ImGuiFileDialog::Instance()->Display("LoadAudio")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                _audioFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
                LoadAudio(_audioFilePath.string());
            }
            ImGuiFileDialog::Instance()->Close();
        }
    } else {
        ImGui::Text("Loaded: %s", _audioFilePath.filename().string().c_str());

        if (ImGui::Button(ICON_CI_PLAY " Play")) {
            Play();
        }

        if (ImGui::Button(ICON_CI_STOP " Stop")) {
            Stop();
        }
    }
}

void Marmalade::ECS::AudioPlayer::Apply(Entity* entity) {
}

void Marmalade::ECS::AudioPlayer::Setup(Entity* entity) {
}

nlohmann::json Marmalade::ECS::AudioPlayer::Serialize(const Entity* entity) {
    nlohmann::json j;

    j["audioFilePath"] = _audioFilePath.string();

    return j;
}

void Marmalade::ECS::AudioPlayer::Deserialize(nlohmann::json json, Entity* entity) {
    _audioFilePath = json["audioFilePath"].get<std::string>();
    LoadAudio(_audioFilePath.string());
}

void Marmalade::ECS::AudioPlayer::LoadAudio(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open audio file: {}", path);
        return;
    }

    char riff[4];
    file.read(riff, 4);
    if (std::strncmp(riff, "RIFF", 4) != 0) {
        LOG_ERROR("Not a RIFF file: {}", path);
        return;
    }

    file.seekg(8, std::ios::beg); // Skip to format
    char wave[4];
    file.read(wave, 4);
    if (std::strncmp(wave, "WAVE", 4) != 0) {
        LOG_ERROR("Not a WAVE file: {}", path);
        return;
    }

    // Read chunks until we find "fmt "
    char chunkId[4];
    int chunkSize = 0;

    short audioFormat = 0;
    short numChannels = 0;
    int sampleRate = 0;
    short bitsPerSample = 0;
    std::vector<char> audioData;

    while (file.read(chunkId, 4)) {
        file.read(reinterpret_cast<char*>(&chunkSize), 4);

        if (std::strncmp(chunkId, "fmt ", 4) == 0) {
            file.read(reinterpret_cast<char*>(&audioFormat), sizeof(short));
            file.read(reinterpret_cast<char*>(&numChannels), sizeof(short));
            file.read(reinterpret_cast<char*>(&sampleRate), sizeof(int));
            file.seekg(6, std::ios::cur); // Skip byte rate (4) + block align (2)
            file.read(reinterpret_cast<char*>(&bitsPerSample), sizeof(short));

            file.seekg(chunkSize - 16, std::ios::cur); // Skip any extra fmt data
        } else if (std::strncmp(chunkId, "data", 4) == 0) {
            audioData.resize(chunkSize);
            file.read(audioData.data(), chunkSize);
        } else {
            // Unknown chunk, skip it
            file.seekg(chunkSize, std::ios::cur);
        }
    }

    if (audioData.empty()) {
        LOG_ERROR("Failed to find audio data in WAV file: {}", path);
        return;
    }

    ALenum format = 0;
    if (numChannels == 1) {
        format = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    } else if (numChannels == 2) {
        format = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    } else {
        LOG_ERROR("Unsupported number of channels: {}", numChannels);
        return;
    }

    if (!_alBuffer) {
        alGenBuffers(1, &_alBuffer);
        alGenSources(1, &_alSource);
    }

    alBufferData(_alBuffer, format, audioData.data(), static_cast<ALsizei>(audioData.size()), sampleRate);
    alSourcei(_alSource, AL_BUFFER, _alBuffer);

    LOG_INFO("Loaded audio file: {} ({} Hz, {} channels, {} bits)", path, sampleRate, numChannels, bitsPerSample);
}

void Marmalade::ECS::AudioPlayer::Play() {
    if (_alSource) {
        LOG_DEBUG("Playing audio: {}", _audioFilePath.string());
        alSourcePlay(_alSource);
    }
}

void Marmalade::ECS::AudioPlayer::Stop() {
    if (_alSource) {
        alSourceStop(_alSource);
    }
}

Marmalade::ECS::AudioPlayer::~AudioPlayer() noexcept {
    if (_alSource) {
        alDeleteSources(1, &_alSource);
    }
    if (_alBuffer) {
        alDeleteBuffers(1, &_alBuffer);
    }
}