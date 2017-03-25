#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>
#include <cmath>
#include "kissfft.hh"
#include "resample.h"
#include "wavwriter.h"
#include "util.h"
#include "shift_pitch.h"

int main(int argc, const char **argv) {
    using namespace std;
    if (argc != 2) {
        return 0;
    }
    string filename(argv[1]);
    MultiChannelData mdata;
    double *data = nullptr;
    size_t length = load_wav(filename, mdata);
    size_t new_length;
    double ratio = 1;
    RectWindow window(4096);
    for (size_t i = 0; i < mdata.size(); ++i) {
        double *old_data = mdata[i];
        double *new_data = nullptr;
        new_length = shift_pitch(old_data, &new_data, ratio, window, length);
        delete old_data;
        mdata[i] = new_data;
    }
    length = new_length;
    size_t mlength = length * mdata.size();
    join_channels(mdata, &data, length);
    write_wav("2.wav", mdata.size(), 44100, data, mlength);
    delete data;
    clear_data(mdata);
    return 0;
}