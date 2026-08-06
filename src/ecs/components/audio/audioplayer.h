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

#ifndef MARMALADE_AUDIOPLAYER_H
#define MARMALADE_AUDIOPLAYER_H

#include "../../component.h"

#include <AL/al.h>

namespace Marmalade::ECS {
    class AudioPlayer : public Component {
    public:
        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;
        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;
        ~AudioPlayer() override;


        void LoadAudio(const std::string& path);

        void Play();
        void Stop();

        AudioPlayer() {
            name = "Audio Player";
            allowMultiple = false;
            categories = {"General", "Audio"};
            description =
                "Creates an audio player that can load audio files and play them";
        }

    private:
        std::filesystem::path _audioFilePath;
        ALuint _alBuffer = 0;
        ALuint _alSource = 0;
    };

    REGISTER_COMPONENT(AudioPlayer);
}

#endif//MARMALADE_AUDIOPLAYER_H
