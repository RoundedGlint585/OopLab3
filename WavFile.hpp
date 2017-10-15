//
// Created by danonofficial on 14.10.17.
//

#ifndef WAV_WAVFILE_HPP
#define WAV_WAVFILE_HPP

#include "WavHeader.h"
#include <vector>
#include <string>

class WavFile {
public:
    WavFile() = delete;

    WavFile(const std::string &filename);

    void printInfo();

    void safeAs(const std::string &filename);

    void makeMono();

    void makeReverb(double delaySeconds, float decay);


private:
    void readHeader(const std::string &filename);

    void checkHeader(const std::string &filename);

    void extractDataInt16(const std::string &filename);

    void recalculateHead(int chanCount, int bitsPerSample, int sampleRate, int samplesCountPerChan);

    std::string name;
    size_t fileSize;
    WavHeader head;
    std::vector<std::vector<short>> data;
};


#endif //WAV_WAVFILE_HPP
