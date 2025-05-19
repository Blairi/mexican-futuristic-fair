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
    if (sounds.count(name)) {
        if (channel) channel->stop();  // Detener el anterior
        system->playSound(sounds[name], nullptr, false, &channel);
    }
}

void AudioManager::stopSound() {
    if (channel) {
        channel->stop();
    }
}

void AudioManager::update() {
    if (system) {
        system->update();
    }
}

void AudioManager::release() {
    for (auto& pair : sounds) {
        pair.second->release();
    }
    sounds.clear();

    if (system) {
        system->close();
        system->release();
        system = nullptr;
    }
}