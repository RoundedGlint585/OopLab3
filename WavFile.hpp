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

    explicit WavFile(const std::string &filename);

    void printInfo();

    void safeAs(const std::string &filename);

    void makeMono();

    void makeReverb(double delaySeconds, float decay);

    unsigned short getAudioFormat();

    unsigned short getNumOfChunnel();

    uint32_t getSampleRate();

    unsigned short getBitsPerSample();


private:
    void readHeader(const std::string &filename);

    void checkHeader(const std::string &filename);

    void extractDataInt16(const std::string &filename);

    void recalculateHead(int chanCount, int bitsPerSample, int sampleRate, int samplesCountPerChan);

    size_t fileSize;
    WavHeader head;
    std::vector<std::vector<short>> data;
};


#endif //WAV_WAVFILE_HPP
