
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

#ifndef MARMALADE_AUDIOMANAGER_H
#define MARMALADE_AUDIOMANAGER_H

#include <AL/al.h>
#include <AL/alc.h>

#include <vector>
#include <string>

class AudioManager {
public:
    static AudioManager& GetInstance();

    ALCdevice* GetCurrentDevice();
    void SetCurrentDevice(const char* deviceName);

    std::vector<std::string> GetAvailableDevices();

private:
    ALCdevice* audioDevice;
    ALCcontext* audioContext;

    AudioManager();
    ~AudioManager();
};


#endif//MARMALADE_AUDIOMANAGER_H
