#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#include "resample.h"
#include "wavwriter.h"

typedef std::vector<double *> MultiChannelData;

size_t load_wav(const std::string &filename, MultiChannelData &data) {
    using namespace std;
    drwav *wav = drwav_open_file(filename.c_str());
    if (!wav) {
        return 0;
    }
    data.clear();
    unsigned int channels = wav->channels;
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
    return real_length;
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

int main(int argc, const char **argv) {
    using namespace std;
    if (argc != 2) {
        return 0;
    }
    string filename(argv[1]);
    MultiChannelData mdata;
    double *data = nullptr;
    size_t length = load_wav(filename, mdata);
    double ratio = 1.1;
    size_t new_length = ceil(length * ratio);
    for (size_t i = 0; i < mdata.size(); ++i) {
        double *old_data = mdata[i];
        double *new_data = new double[new_length];
        resample_linear(old_data, new_data, ratio, length, new_length);
        delete old_data;
        mdata[i] = new_data;
    }
    size_t mlength = new_length * mdata.size();
    join_channels(mdata, &data, length);
    write_wav("2.wav", mdata.size(), 44100, data, mlength);
    delete data;
    for (MultiChannelData::iterator i = mdata.begin(); i < mdata.end(); ++i) {
        delete *i;
    }
    return 0;
}