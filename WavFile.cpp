//
// Created by danonofficial on 14.10.17.
//

#include "WavFile.hpp"
#include <iostream>
#include "Exceptions/IOException.hpp"
#include "Exceptions/BadFormatException.hpp"
#include "Exceptions/HeaderException.hpp"
#include "Exceptions/UnsupportedFormatException.hpp"


WavFile::WavFile(const std::string &filename) {
    readHeader(filename);
    extractDataInt16(filename);
}

void WavFile::printInfo() {
    printf("-------------------------\n");
    printf(" audioFormat   %u\n", head.audioFormat);
    printf(" numChannels   %u\n", head.numChannels);
    printf(" sampleRate    %u\n", head.sampleRate);
    printf(" bitsPerSample %u\n", head.bitsPerSample);
    printf(" byteRate      %u\n", head.byteRate);
    printf(" blockAlign    %u\n", head.blockAlign);
    printf(" chunkSize     %u\n", head.chunkSize);
    printf(" subchunk1Size %u\n", head.subchunk1Size);
    printf(" subchunk2Size %u\n", head.subchunk2Size);
    printf("-------------------------\n");
}

void WavFile::readHeader(const std::string &filename) {
    std::clog << "Reading Header\n";
    //null_header( header_ptr); // Fill header with zeroes.

    FILE *f = fopen(filename.c_str(), "rb");
    if (!f) {
        throw error::IOException("No such file: " + filename);
    }
    std::clog << "Size of head: " << sizeof(head) << std::endl;
    size_t blocks_read = fread(&head, sizeof(head), 1, f);
    if (blocks_read != 1) {
        // can't read header, because the file is too small.
        throw error::BadFormatException("File is too small");
    }

    fseek(f, 0, SEEK_END); // seek to the end of the file
    fileSize = ftell(f); // current position is a file size!
    fclose(f);
    std::clog << "Complete Reading\nStart checking\n";
    checkHeader(filename);
    std::clog << "Checking complete. It's OK\n";
}

void WavFile::checkHeader(const std::string &filename) {
    if (head.chunkId[0] != 'R' ||
        head.chunkId[1] != 'I' ||
        head.chunkId[2] != 'F' ||
        head.chunkId[3] != 'F') {
        throw error::HeaderException("Haven't RIFF in Header");
    }
    if (head.chunkSize != fileSize - 8) {
        throw error::HeaderException("Wrong header size");
    }

    if (head.format[0] != 'W' ||
        head.format[1] != 'A' ||
        head.format[2] != 'V' ||
        head.format[3] != 'E') {
        throw error::HeaderException("Haven't WAVE in Header");
    }

    if (head.subchunk1Id[0] != 'f' ||
        head.subchunk1Id[1] != 'm' ||
        head.subchunk1Id[2] != 't' ||
        head.subchunk1Id[3] != ' ') {
        throw error::HeaderException("Haven't fmt in Header");
    }

    if (head.audioFormat != 1) {
        throw error::HeaderException("AudioFormat is not 1. Only PCM.");
    }

    if (head.subchunk1Size != 16) {
        throw error::HeaderException("Subchunk is not 16. Only PCM");
    }

    if (head.byteRate != head.sampleRate * head.numChannels * head.bitsPerSample / 8) {
        throw error::HeaderException("Wrong byteRate");
    }

    if (head.blockAlign != head.numChannels * head.bitsPerSample / 8) {
        throw error::HeaderException("Wrong block align");
    }

    if (head.subchunk2Id[0] != 'd' ||
        head.subchunk2Id[1] != 'a' ||
        head.subchunk2Id[2] != 't' ||
        head.subchunk2Id[3] != 'a') {
        throw error::HeaderException("Haven't data in Header");
    }

    if (head.subchunk2Size != fileSize - 44) {
        throw error::HeaderException("Wrong data size");
    }

}

void WavFile::extractDataInt16(const std::string &filename) {
    std::clog << "Starting loading data\n";
    if (head.bitsPerSample != 16) {
        // Only 16-bit samples is supported.
        throw error::UnsupportedFormatException("Unknown format. Only 16-bit support");
    }

    FILE *f = fopen(filename.c_str(), "rb");
    if (!f) {
        throw error::IOException("No such file");
    }
    fseek(f, 44, SEEK_SET); // Seek to the begining of PCM data.

    int chanCount = head.numChannels;
    int samplesPerChan = (head.subchunk2Size / sizeof(short)) / chanCount;

    // 1. Reading all PCM data from file to a single vector.
    std::vector<short> allChannels;
    allChannels.resize(chanCount * samplesPerChan);
    size_t readBytes = fread(allChannels.data(), 1, head.subchunk2Size, f);
    if (readBytes != head.subchunk2Size) {
        printf("extract_data_int16() read only %zu of %u\n", readBytes, head.subchunk2Size);
        throw error::IOException(
                "Read only" + std::to_string(readBytes) + " of " + std::to_string(head.subchunk2Size));
    }
    fclose(f);


    // 2. Put all channels to its own vector.
    data.resize(chanCount);
    for (size_t ch = 0; ch < data.size(); ch++) {
        data[ch].resize(samplesPerChan);
    }

    for (int ch = 0; ch < chanCount; ch++) {
        std::vector<short> &chdata = data[ch];
        for (size_t i = 0; i < samplesPerChan; i++) {
            chdata[i] = allChannels[chanCount * i + ch];
        }
    }
    std::clog << "Loading complete. It's OK\n";
}

void WavFile::safeAs(const std::string &filename) {
    int chanCount = head.numChannels;
    int samplesPerChan = (head.subchunk2Size / sizeof(short)) / chanCount;
    std::vector<short> allChannels;
    allChannels.resize(chanCount * samplesPerChan);

    for (int ch = 0; ch < chanCount; ch++) {
        const std::vector<short> &chdata = data[ch];
        for (size_t i = 0; i < samplesPerChan; i++) {
            allChannels[chanCount * i + ch] = chdata[i];
        }
    }
    FILE *f = fopen(filename.c_str(), "wb");
    fwrite(&head, sizeof(head), 1, f);
    std::clog << data[0].size() << " " << data[1].size() << std::endl;
    fwrite(data.data(), sizeof(short), data[0].size() * 2, f);
    if (!f) {
        throw error::IOException("Can't write Wav-file");
    }
    fclose(f);
}

void WavFile::makeMono() {
    std::clog << "Start making mono sound\n";
    int chanCount = (int) data.size();
    std::clog << "Detected " + std::to_string(chanCount) + " channels\n";
    if (chanCount == 1) {
        std::clog << "File alread in mono\n";
        return;
    }

    int samplesCountPerChan = (int) data[0].size();

    // Verify that all channels have the same number of samples.
    for (size_t ch = 0; ch < chanCount; ch++) {
        if (data[ch].size() != (size_t) samplesCountPerChan) {
            throw error::BadFormatException("Wrong samples amount in channel");
        }
    }

    std::vector<std::vector<short>> res;
    res.resize(1);
    res[0].resize(samplesCountPerChan);
    std::clog << res.size() << " " << res[0].size() << std::endl;
    // Mono channel is an arithmetic mean of all (two) channels.
    for (size_t i = 0; i < samplesCountPerChan; i++) {
        for (int j = 0; j < chanCount; j++) {
            res[0][i] += data[j][i];
        }
        res[0][i] /= chanCount;
    }
    this->data = res;
    recalculateHead(1, head.bitsPerSample, head.sampleRate, samplesCountPerChan);
    std::clog << "Complete making mono\n";
}

void WavFile::recalculateHead(int chanCount, int bitsPerSample, int sampleRate, int samplesCountPerChan) {
    std::clog << "Updating Header\n";
    head.sampleRate = sampleRate;
    head.numChannels = chanCount;
    head.bitsPerSample = 16;
    int fileSizeBytes = 44 + chanCount * (bitsPerSample / 8) * samplesCountPerChan;
    head.chunkSize = fileSizeBytes - 8;
    head.subchunk2Size = fileSizeBytes - 44;

    head.byteRate = head.sampleRate * head.numChannels * head.bitsPerSample / 8;
    head.blockAlign = head.numChannels * head.bitsPerSample / 8;

    std::clog << "Updating complete\n";
}

void WavFile::makeReverb(double delaySeconds, float decay) {
    int chan_count = (int) data.size();
    int sampleRate = head.sampleRate;
    if (chan_count < 1) {
        throw error::BadFormatException("Only one channel");
    }

    int samplesCountPerChan = (int) data[0].size();

    // Verify that all channels have the same number of samples.
    for (size_t ch = 0; ch < chan_count; ch++) {
        if (data[ch].size() != (size_t) samplesCountPerChan) {
            throw error::BadFormatException("Wrong samples amount in channel");
        }
    }

    int delaySamples = (int) (delaySeconds * sampleRate);


    for (size_t ch = 0; ch < chan_count; ch++) {
        std::vector<float> tmp;
        tmp.resize(data[ch].size());

        // Convert signal from short to float
        for (size_t i = 0; i < samplesCountPerChan; i++) {
            tmp[i] = data[ch][i];
        }

        // Add a reverb
        for (size_t i = 0; i < samplesCountPerChan - delaySamples; i++) {
            tmp[i + delaySamples] += decay * tmp[i];
        }

        // Find maximum signal's magnitude
        float max_magnitude = 0.0f;
        for (size_t i = 0; i < samplesCountPerChan - delaySamples; i++) {
            if (abs(tmp[i]) > max_magnitude) {
                max_magnitude = abs(tmp[i]);
            }
        }

        // Signed short can keep values from -32768 to +32767,
        // After reverb, usually there are values large 32000.
        // So we must scale all values back to [ -32768 ... 32768 ]
        float norm_coef = 30000.0f / max_magnitude;
        printf("max_magnitude = %.1f, coef = %.3f\n", max_magnitude, norm_coef);

        // Scale back and transform floats to shorts.
        for (size_t i = 0; i < samplesCountPerChan; i++) {
            data[ch][i] = (short) (norm_coef * tmp[i]);
        }
    }

}
