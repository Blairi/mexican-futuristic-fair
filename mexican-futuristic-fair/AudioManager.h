#pragma once
#include <fmod.hpp>
#include <map>
#include <string>

class AudioManager {
public:
    void init();
    void loadSound(const std::string& name, const std::string& filepath, bool loop);
    void playSound(const std::string& name);
    void stopSound(const std::string& name);
    void stopAllSounds();
    void setVolume(const std::string& name, float volume);
    void update();
    void release();
    bool isSoundPlaying(const std::string& name);

private:
    FMOD::System* system = nullptr;
    std::map<std::string, FMOD::Sound*> sounds;
    std::map<std::string, FMOD::Channel*> channels;
};
