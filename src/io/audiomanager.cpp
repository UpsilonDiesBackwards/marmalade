
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

#include "io/audiomanager.h"

#include "../application/logger.h"
#include "../application/config/engineconfig.h"

#include <sstream>
#include <iostream>

AudioManager::AudioManager() {
    audioDevice = alcOpenDevice(Marmalade::EngineConfig::GetStoredConfig().audioOutputDevice.c_str());
    if (audioDevice) {
        std::cout << "Using audio device: " << alcGetString(audioDevice, ALC_DEFAULT_ALL_DEVICES_SPECIFIER) << std::endl;
    } else {
        audioDevice = alcOpenDevice(nullptr);
    }

    audioContext = alcCreateContext(audioDevice, nullptr);
    if (audioContext) {
        std::cout << "Created OpenAL Context" << std::endl;
    }

    ALCenum alError = alcGetError(audioDevice);
    if (alError != ALC_NO_ERROR) {
        LOG_ERROR("OpenAL error: {}", alError);
    }

    alcMakeContextCurrent(audioContext);
}

AudioManager::~AudioManager() {
    std::cout << "Closing OpenAL context"<< std::endl;

    alcMakeContextCurrent(nullptr);
    alcDestroyContext(audioContext);
    alcCloseDevice(audioDevice);
}

AudioManager& AudioManager::GetInstance(){
    static AudioManager instance;
    return instance;
}

ALCdevice* AudioManager::GetCurrentDevice() {
    return audioDevice;
}

void AudioManager::SetCurrentDevice(const char* deviceName) {
    std::string currentDeviceName = alcGetString(alcGetContextsDevice(audioContext), ALC_DEVICE_SPECIFIER);
    if (currentDeviceName == deviceName) {
        return;
    }

    if (audioContext) {
        ALCdevice* currentDevice = alcGetContextsDevice(audioContext);

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(audioContext);

        if (currentDevice) {
            alcCloseDevice(currentDevice);
        }
    }

    ALCdevice* newDevice = alcOpenDevice(deviceName);
    if (!newDevice) {
        std::cerr << "Failed to open new device: " << deviceName << std::endl;
        return;
    }

    ALCcontext* newContext = alcCreateContext(newDevice, nullptr);
    if (!newContext) {
        std::cerr << "Failed to create context for new device: " << deviceName << std::endl;
        alcCloseDevice(newDevice);
        return;
    }

    if (!alcMakeContextCurrent(newContext)) {
        std::cerr << "Failed to make new context current for device: " << deviceName << std::endl;
        alcDestroyContext(newContext);
        alcCloseDevice(newDevice);
        return;
    }

    audioDevice = newDevice;
    audioContext = newContext;

    std::cout << "Successfully switched to device: " << deviceName << std::endl;
}

std::vector<std::string> AudioManager::GetAvailableDevices() {
    std::vector<std::string> availableDevices;

    const char* deviceList = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);

    if (deviceList != nullptr) {
        std::istringstream ss(deviceList);
        std::string device;

        while (std::getline(ss, device, ',')) {
            availableDevices.push_back(device);
        }
    } else {
        LOG_ERROR("No audio devices found on system");
    }

    return availableDevices;
}
