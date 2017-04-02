#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cmath>
#include "kissfft.hh"
#include "resample.h"
#include "wavwriter.h"
#include "util.h"
#include "shift_pitch.h"
#include "filter.h"

void print_usage() {
    std::cout <<
        "Usage:\n" <<
        "./audio-tool resample-linear in.wav out.wav ratio output-sample-rate\n" <<
        "./audio-tool shift-pitch in.wav out.wav ratio [window-type=rect] [window-size=4096]\n" <<
        "    Available window-type: rect(default), hann, blackman\n" <<
        "./audio-tool fft-filter in.wav out.wav profile\n" <<
        "./audio-tool generate-fft-filter-profile-lowpass out cutoff-frequency sample-rates window-type window-size\n" <<
        "./audio-tool usage\n";
}

void command_shift_pitch(const std::string &in, const std::string &out,
                         double ratio, const std::string &window_type, size_t window_size) {
    MultiChannelData mdata;
    double *data = nullptr;
    WavInfo info = load_wav(in, mdata);
    if (info.length == 0) {
        std::cout << "No such file.\n";
        return;
    }
    size_t length = info.length / info.channels;
    size_t new_length;
    WindowFunction *window = nullptr;
    if (window_type == "rect") {
        window = new RectWindow(window_size);
    } else if (window_type == "hann") {
        window = new HannWindow(window_size);
    } else if (window_type == "blackman") {
        window = new BlackmanWindow(window_size);
    } else {
        std::cout << "No such window type.\n";
        return;
    }
    for (size_t i = 0; i < mdata.size(); ++i) {
        double *old_data = mdata[i];
        double *new_data = nullptr;
        if (window_type == "rect") {
            new_length = shift_pitch(old_data, &new_data, ratio, dynamic_cast<RectWindow &>(*window), length);
        } else {
            new_length = shift_pitch(old_data, &new_data, ratio, *window, length);
        }
        delete old_data;
        mdata[i] = new_data;
    }
    length = new_length;
    size_t mlength = length * mdata.size();
    join_channels(mdata, &data, length);
    info.length = mlength;
    write_wav(out, info, data);
    delete data;
    delete window;
    clear_data(mdata);
}

void command_fft_filter(const std::string &in, const std::string &out, const std::string &profile) {
    using namespace std;
    MultiChannelData mdata;
    double *data = nullptr;
    WavInfo info = load_wav(in, mdata);
    if (info.length == 0) {
        cout << "No such file.\n";
        return;
    }
    size_t length = info.length / info.channels;
    ifstream f_profile(profile);
    if (!f_profile) {
        cout << "No such profile.\n";
        return;
    }
    string window_type;
    int window_size = 0;
    if (!(f_profile >> window_type >> window_size)) {
        cout << "Invalid profile.\n";
        return;
    }
    vector<double> factors;
    for (double factor; f_profile >> factor; factors.push_back(factor));
    f_profile.close();
    if (factors.size() != (window_size / 2 + 1)) {
        cout << "Invalid profile.\n";
        return;
    }
    WindowFunction *window = nullptr;
    if (window_type == "rect") {
        window = new RectWindow(window_size);
    } else if (window_type == "hann") {
        window = new HannWindow(window_size);
    } else if (window_type == "blackman") {
        window = new BlackmanWindow(window_size);
    } else {
        cout << "No such window type.\n";
        return;
    }
    for (size_t i = 0; i < mdata.size(); ++i) {
        double *old_data = mdata[i];
        double *new_data = nullptr;
        fft_filter(old_data, &new_data, *window, length, factors);
        delete old_data;
        mdata[i] = new_data;
    }
    size_t mlength = length * mdata.size();
    join_channels(mdata, &data, length);
    info.length = mlength;
    write_wav(out, info, data);
    delete data;
    delete window;
    clear_data(mdata);
}

void command_generate_fft_filter_profile_lowpass(const std::string &out, double frequency, int sample_rates,
                                                 const std::string &window_type, int window_size) {
    using namespace std;
    ofstream f(out);
    int k = frequency * window_size / sample_rates;
    if (k > window_size / 2 + 1) {
        cout << "Cutoff frequency is higher than Nyquist frequency.\n";
    }
    if (f) {
        f << window_type << " " << window_size << endl;
    }
    for (int i = 0; i < window_size / 2 + 1; ++i) {
        if (i <= k) {
            f << 1.0 << " ";
        } else {
            f << 0.0 << " ";
        }
    }
    f.close();
}

void command_resample_linear(const std::string &in, const std::string &out,
                             double ratio, int sample_rate) {
    MultiChannelData mdata;
    double *data = nullptr;
    WavInfo info = load_wav(in, mdata);
    if (info.length == 0) {
        std::cout << "No such file.\n";
        return;
    }
    size_t length = info.length / info.channels;
    size_t new_length = ceil(length * ratio);
    for (size_t i = 0; i < mdata.size(); ++i) {
        double *old_data = mdata[i];
        double *new_data = new double[new_length];
        resample_linear(old_data, new_data, ratio, length, new_length);
        delete old_data;
        mdata[i] = new_data;
    }
    length = new_length;
    size_t mlength = length * mdata.size();
    join_channels(mdata, &data, length);
    info.length = mlength;
    info.sample_rate = sample_rate;
    write_wav(out, info, data);
    delete data;
    clear_data(mdata);
}

int main(int argc, const char **argv) {
    using namespace std;
    if (argc < 2) {
        print_usage();
        return 0;
    }
    string command = argv[1];
    if (command == "usage") {
        print_usage();
    } else if (command == "shift-pitch") {
        if (argc != 5 && argc != 6 && argc != 7) {
            print_usage();
            return 0;
        }
        string in = argv[2];
        string out = argv[3];
        double ratio = atof(argv[4]);
        string window_type = "rect";
        size_t window_size = 4096;
        if (argc > 5) {
            window_type = argv[5];
        }
        if (argc > 6) {
            window_size = atoi(argv[6]);
        }
        command_shift_pitch(in, out, ratio, window_type, window_size);
    } else if (command == "resample-linear") {
        if (argc != 6) {
            print_usage();
            return 0;
        }
        string in = argv[2];
        string out = argv[3];
        double ratio = atof(argv[4]);
        string window_type = "rect";
        int sample_rate = atoi(argv[5]);
        command_resample_linear(in, out, ratio, sample_rate);
    } else if (command == "fft-filter") {
        if (argc != 5) {
            print_usage();
            return 0;
        }
        string in = argv[2];
        string out = argv[3];
        string profile = argv[4];
        command_fft_filter(in, out, profile);
    } else if (command == "generate-fft-filter-profile-lowpass") {
        if (argc != 7) {
            print_usage();
            return 0;
        }
        string out = argv[2];
        double frequency = atof(argv[3]);
        int sample_rates = atoi(argv[4]);
        string window_type = argv[5];
        int window_size = atoi(argv[6]);
        command_generate_fft_filter_profile_lowpass(out, frequency, sample_rates, window_type, window_size);
    } else {
        print_usage();
    }
    return 0;
}