//
// Created by EBao on 2019-05-07.
//

#include "Audio.h"

bool Audio::isBigEndian()
{
    int a = 1;
    return !((char*)&a)[0];
}

int Audio::convertToInt(char* buffer, int len)
{
    int a = 0;
    if (!isBigEndian())
        for (int i = 0; i<len; i++)
            ((char*)&a)[i] = buffer[i];
    else
        for (int i = 0; i<len; i++)
            ((char*)&a)[3 - i] = buffer[i];
    return a;
}

char* Audio::loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size)
{
    char buffer[4];
    std::ifstream in(fn, std::ios::binary);
    in.read(buffer, 4);
    if (strncmp(buffer, "RIFF", 4) != 0)
    {
        std::cout << "this is not a valid WAVE file" << std::endl;
        return NULL;
    }
    in.read(buffer, 4);
    in.read(buffer, 4);      //WAVE
    in.read(buffer, 4);      //fmt
    in.read(buffer, 4);      //16
    in.read(buffer, 2);      //1
    in.read(buffer, 2);
    chan = convertToInt(buffer, 2);
    in.read(buffer, 4);
    samplerate = convertToInt(buffer, 4);
    in.read(buffer, 4);
    in.read(buffer, 2);
    in.read(buffer, 2);
    bps = convertToInt(buffer, 2);
    in.read(buffer, 4);      //data
    in.read(buffer, 4);
    size = convertToInt(buffer, 4);
    char* data = new char[size];
    in.read(data, size);
    return data;
}


Audio::Audio(std::string file) {
    data = loadWAV(file.c_str(), channel, sampleRate, bps, size);
    device = alcOpenDevice(NULL);
    if (device == NULL)
    {
        std::cout << "cannot open sound card" << std::endl;
        return;
    }
    context = alcCreateContext(device, NULL);
    if (context == NULL)
    {
        std::cout << "cannot open context" << std::endl;
        return;
    }
    alcMakeContextCurrent(context);

    alGenBuffers(1, &bufferid);

    if (channel == 1)
    {
        if (bps == 8)
        {
            format = AL_FORMAT_MONO8;
        }
        else {
            format = AL_FORMAT_MONO16;
        }
    }
    else {
        if (bps == 8)
        {
            format = AL_FORMAT_STEREO8;
        }
        else {
            format = AL_FORMAT_STEREO16;
        }
    }
    alBufferData(bufferid, format, data, size, sampleRate);
    alGenSources(1, &sourceid);
    alSourcei(sourceid, AL_BUFFER, bufferid);
}

void Audio::play() {
    alSourcePlay(sourceid);
}

Audio::~Audio() {
    alDeleteSources(1, &sourceid);
    alDeleteBuffers(1, &bufferid);

    alcDestroyContext(context);
    alcCloseDevice(device);
    delete[] data;
}