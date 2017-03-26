#include <string>
#include <vector>
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#include "util.h"

WavInfo load_wav(const std::string &filename, MultiChannelData &data) {
    using namespace std;
    drwav *wav = drwav_open_file(filename.c_str());
    if (!wav) {
        return {0, 0, 0, 0, 0};
    }
    data.clear();
    unsigned short channels = wav->channels;
    size_t length = wav->totalSampleCount;
    size_t real_length = length / channels;
    float *sample_data = new float[length];
    drwav_read_f32(wav, length, sample_data);
    for (unsigned int i = 0; i < channels; ++i) {
        double *double_data = new double[real_length];
        for (size_t j = 0; j < real_length; ++j) {
            double_data[j] = sample_data[j*channels+i];
        }
        data.push_back(double_data);
    }
    delete sample_data;
    drwav_close(wav);
    return {
        length,
        wav->sampleRate,
        channels,
        wav->bitsPerSample,
        wav->bytesPerSample
    };
}

size_t join_channels(const MultiChannelData &in, double **out, size_t length) {
    unsigned int channels = in.size();
    *out = new double[channels*length];
    for (unsigned int i = 0; i < channels; ++i) {
        for (size_t j = 0; j < length; ++j) {
            double *data = in[i];
            (*out)[j*channels+i] = *(data + j);
        }
    }
    return length * channels;
}

void clear_data(MultiChannelData &data) {
    for (MultiChannelData::iterator i = data.begin(); i < data.end(); ++i) {
        delete *i;
    }
}