#include <iostream>
#include <fstream>
#include <cstring>
#include "wavwriter.h"

struct RIFFHeader {
    uint8_t id[4];
    uint32_t size;
    uint8_t type[4];
};

struct FormatChunk {
    uint8_t id[4];
    uint32_t size;
    uint16_t format_tag;
    uint16_t channels;
    uint32_t samples_per_second;
    uint32_t bytes_per_second;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

struct DataChunkHeader {
    uint8_t id[4];
    uint32_t size;
};

void write_wav(const std::string &filename, const WavInfo &info, double *data) {
    using namespace std;
    size_t length = info.length;
    size_t file_length = length * info.bytes_per_sample + sizeof(RIFFHeader) + sizeof(FormatChunk) + 8;
    RIFFHeader riff_header = {
        {'R', 'I', 'F', 'F'},
        (uint32_t)(file_length - 8),
        {'W', 'A', 'V', 'E'}
    };
    FormatChunk format_chunk = {
        {'f', 'm', 't', ' '},
        16,
        (unsigned short)(info.bits_per_sample != 32 ? 0x001 : 0x003),
        (uint16_t)info.channels,
        info.sample_rate,
        info.sample_rate * info.channels * info.bytes_per_sample,
        info.bytes_per_sample,
        info.bits_per_sample
    };
    DataChunkHeader data_chunk_header = {
        {'d', 'a', 't', 'a'},
        (uint32_t)(length * info.bytes_per_sample)
    };
    ofstream of(filename);
    of.write((char *)&riff_header, sizeof(RIFFHeader));
    of.write((char *)&format_chunk, sizeof(FormatChunk));
    of.write((char *)&data_chunk_header, sizeof(DataChunkHeader));
    if (info.bits_per_sample == 16) {
        int16_t *buf = new int16_t[length];
        for (size_t i = 0; i < length; ++i) {
            buf[i] = (int16_t)(data[i] * 32768);
        }
        of.write((char *)buf, length * sizeof(int16_t));
        delete buf;
    } else if (info.bits_per_sample == 32) {
        float *buf = new float[length];
        for (size_t i = 0; i < length; ++i) {
            buf[i] = data[i];
        }
        of.write((char *)buf, length * sizeof(float));
        delete buf;
    } else if (info.bits_per_sample == 24) {
        /*
        TODO: support for 24bit wav
        char *buf = new char[length*3];
        for (size_t i = 0; i < length; ++i) {
            int32_t v = data[i] * 8388608;
            //printf("%d\n", v);
            char *v_bytes = (char *)&v;
            buf[i*3] = (v & 0x80000000 >> 8) || v_bytes[1];
            buf[i*3+1] = v_bytes[2];
            buf[i*3+2] = v_bytes[3];
        }
        of.write(buf, length * 3);
        delete buf;
        */
    }
    of.close();
}