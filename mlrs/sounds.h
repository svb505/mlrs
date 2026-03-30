#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <iostream>
#include <vector>

class Sound {
public:
    ALCdevice* audioDevice = nullptr;
    ALCcontext* audioContext = nullptr;

    ALuint shootBuffer = 0;
    ALuint turretBuffer = 0;
    ALuint explosionBuffer = 0;
    ALuint storageExplosionBuffer = 0;

    ALuint shootSource = 0;
    ALuint turretSource = 0;
    ALuint explosionSource = 0;
    ALuint storageExplosionSource = 0;

    ALuint LoadWav(const char* filename) {
        SF_INFO info{};
        SNDFILE* file = sf_open(filename, SFM_READ, &info);
        if (!file) {
            std::cerr << "Failed to load sound: " << filename << "\n";
            return 0;
        }

        std::vector<short> samples(info.frames * info.channels);
        sf_readf_short(file, samples.data(), info.frames);
        sf_close(file);

        ALuint buffer;
        alGenBuffers(1, &buffer);

        ALenum format = (info.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

        alBufferData(buffer, format, samples.data(), (ALsizei)(samples.size() * sizeof(short)), info.samplerate);

        return buffer;
    }
    void setSourcePosition(ALuint source, float x, float y, float z) {
        ALfloat pos[] = { x, y, z };
        ALfloat vel[] = { 0.0f, 0.0f, 0.0f };

        alSourcefv(source, AL_POSITION, pos);
        alSourcefv(source, AL_VELOCITY, vel);
    }
    void setListener(float x, float y, float z, float fx, float fy, float fz) {
        ALfloat pos[] = { x, y, z };
        ALfloat vel[] = { 0.0f, 0.0f, 0.0f };

        ALfloat ori[] = {
            fx, fy, fz,
            0.0f, 1.0f, 0.0f
        };

        alListenerfv(AL_POSITION, pos);
        alListenerfv(AL_VELOCITY, vel);
        alListenerfv(AL_ORIENTATION, ori);
    }
    void setupBuffers() {
        audioDevice = alcOpenDevice(nullptr);
        audioContext = alcCreateContext(audioDevice, nullptr);
        alcMakeContextCurrent(audioContext);

        shootBuffer = LoadWav("sounds/zalp.wav");
        turretBuffer = LoadWav("sounds/rotation.wav");
        explosionBuffer = LoadWav("sounds/explosion.wav");
        storageExplosionBuffer = LoadWav("sounds/storageExplosion.wav");
    }
    void createSources() {
        alGenSources(1, &turretSource);
        alSourcei(turretSource, AL_BUFFER, turretBuffer);
        alSourcef(turretSource, AL_GAIN, 0.3f);

        alGenSources(1, &shootSource);
        alSourcei(shootSource, AL_BUFFER, shootBuffer);
        alSourcef(shootSource, AL_GAIN, 0.5f);

        alGenSources(1, &explosionSource);
        alSourcei(explosionSource, AL_BUFFER, explosionBuffer);
        alSourcef(explosionSource, AL_GAIN, 1.0f);

        alGenSources(1, &storageExplosionSource);
        alSourcei(storageExplosionSource, AL_BUFFER, storageExplosionBuffer);
        alSourcef(storageExplosionSource, AL_GAIN, 1.0f);

        alSourcef(shootSource, AL_REFERENCE_DISTANCE, 5.0f);
        alSourcef(shootSource, AL_MAX_DISTANCE, 90.0f);
        alSourcef(shootSource, AL_ROLLOFF_FACTOR, 1.0f);

        alSourcef(explosionSource, AL_REFERENCE_DISTANCE, 2.0f);
        alSourcef(explosionSource, AL_MAX_DISTANCE, 30.0f);
        alSourcef(explosionSource, AL_ROLLOFF_FACTOR, 2.0f);

        alSourcef(storageExplosionSource, AL_REFERENCE_DISTANCE, 2.0f);
        alSourcef(storageExplosionSource, AL_MAX_DISTANCE, 30.0f);
        alSourcef(storageExplosionSource, AL_ROLLOFF_FACTOR, 2.0f);
    }
};

