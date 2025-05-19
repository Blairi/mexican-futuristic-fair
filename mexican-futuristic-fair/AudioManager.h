#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <fmod.hpp>
#include <map>
#include <string>

class AudioManager {
private:
    FMOD::System* system = nullptr;
    std::map<std::string, FMOD::Sound*> sounds;
    FMOD::Channel* channel = nullptr;

public:
    void init();
    void loadSound(const std::string& name, const std::string& filepath, bool loop = false);
    void playSound(const std::string& name);
    void stopSound();
    void update();
    void release();
};

#endif // AUDIOMANAGER_H
