//
// Created by EBao on 2019-05-07.
//

#ifndef CLOTHSIM_AUDIO_H
#define CLOTHSIM_AUDIO_H

#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <string>
#include <iostream>
#include <fstream>

class Audio {

public:
    Audio(std::string file);
    ~Audio();
    void play();

private:
    char* loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size);
    int convertToInt(char* buffer, int len);
    bool isBigEndian();

    int channel, sampleRate, bps, size;
    ALCdevice* device;
    ALCcontext* context;
    unsigned int bufferid, format;
    unsigned int sourceid;
    char* data;
};


#endif //CLOTHSIM_AUDIO_H
