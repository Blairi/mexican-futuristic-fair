#include "AudioManager.h"
#include <iostream>

void AudioManager::init() {
    FMOD_RESULT result;

    result = FMOD::System_Create(&system);
    if (result != FMOD_OK) {
        std::cerr << "Error creando el sistema FMOD: " << result << "\n";
        return;
    }

    result = system->init(512, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) {
        std::cerr << "Error inicializando FMOD: " << result << "\n";
    }
}

void AudioManager::loadSound(const std::string& name, const std::string& filepath, bool loop) {
    FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;
    FMOD::Sound* sound = nullptr;

    FMOD_RESULT result = system->createSound(filepath.c_str(), mode, nullptr, &sound);
    if (result != FMOD_OK) {
        std::cerr << "No se pudo cargar el sonido: " << filepath << "\n";
        return;
    }

    sounds[name] = sound;
}

void AudioManager::playSound(const std::string& name) {
    auto it = sounds.find(name);
    if (it == sounds.end()) {
        std::cerr << "El sonido '" << name << "' no ha sido cargado.\n";
        return;
    }

    FMOD::Channel* newChannel = nullptr;
    FMOD_RESULT result = system->playSound(it->second, nullptr, false, &newChannel);
    if (result != FMOD_OK) {
        std::cerr << "Error al reproducir el sonido '" << name << "'\n";
        return;
    }

    channels[name] = newChannel;
}

void AudioManager::stopSound(const std::string& name) {
    auto it = channels.find(name);
    if (it != channels.end() && it->second) {
        it->second->stop();
        it->second = nullptr;
    }
}

void AudioManager::stopAllSounds() {
    for (auto& pair : channels) {
        if (pair.second) {
            pair.second->stop();
            pair.second = nullptr;
        }
    }
}

void AudioManager::update() {
    if (system) {
        system->update();
    }
}

void AudioManager::release() {
    stopAllSounds();  // Detener todos los sonidos antes de liberar

    for (auto& pair : sounds) {
        if (pair.second) {
            pair.second->release();
        }
    }
    sounds.clear();
    channels.clear();

    if (system) {
        system->close();
        system->release();
        system = nullptr;
    }
}

void AudioManager::setVolume(const std::string& name, float volume) {
    if (channels.find(name) != channels.end() && channels[name]) {
        channels[name]->setVolume(volume);
    }
}


bool AudioManager::isSoundPlaying(const std::string& name) {
    if (channels.find(name) != channels.end() && channels[name]) {
        bool isPlaying = false;
        channels[name]->isPlaying(&isPlaying);
        return isPlaying;
    }
    return false;
}