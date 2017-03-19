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

void write_wav(const std::string &filename,
               unsigned int channels, unsigned int samples_per_second,
               double *data, size_t length) {
    using namespace std;
    size_t file_length = length * 2 + sizeof(RIFFHeader) + sizeof(FormatChunk) + 8;
    RIFFHeader riff_header = {
        {'R', 'I', 'F', 'F'},
        (uint32_t)(file_length - 8),
        {'W', 'A', 'V', 'E'}
    };
    FormatChunk format_chunk = {
        {'f', 'm', 't', ' '},
        16,
        0x0001,
        (uint16_t)channels,
        samples_per_second,
        samples_per_second * channels * 2,
        2,
        16
    };
    DataChunkHeader data_chunk_header = {
        {'d', 'a', 't', 'a'},
        (uint32_t)(length * 2)
    };
    int16_t *buf = new int16_t[length];
    for (size_t i = 0; i < length; ++i) {
        buf[i] = (int16_t)(data[i] * 32768);
    }
    ofstream of(filename);
    of.write((char *)&riff_header, sizeof(RIFFHeader));
    of.write((char *)&format_chunk, sizeof(FormatChunk));
    of.write((char *)&data_chunk_header, sizeof(DataChunkHeader));
    of.write((char *)buf, length * sizeof(int16_t));
    of.close();
    delete buf;
}