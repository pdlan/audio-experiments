#include <iostream>
#include <string>
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>
#include "kissfft.hh"
#include "resample.h"
#include "wavwriter.h"
#include "util.h"

size_t shift_pitch(const double *in, double **out, double ratio, size_t length) {
    using namespace std;
    const size_t WindowSize = 4096;
    size_t window_number = ceil(double(length) / WindowSize);
    size_t new_length = window_number * WindowSize;
    *out = new double[new_length];
    kissfft<double> fft(WindowSize, false);
    kissfft<double> fft_inverse(WindowSize, true);
    typedef kissfft<double>::cpx_t ComplexType;
    for (size_t i = 0; i < window_number; ++i) {
        bool need_delete = false;
        const double *window_data = nullptr;
        if (i < (window_number - 1)) {
            window_data = in + i * WindowSize;
        } else {
            need_delete = true;
            window_data = new double[WindowSize];
            size_t length_left = length - WindowSize * i;
            copy(in + i * WindowSize,
                 in + i * WindowSize + length_left - 1,
                 (double *)window_data);
        }
        ComplexType *window_data_complex = new ComplexType[WindowSize];
        ComplexType *fd = new ComplexType[WindowSize];
        ComplexType *inversed = new ComplexType[WindowSize];
        for (size_t j = 0; j < WindowSize; ++j) {
            window_data_complex[j].real(window_data[j]);
            window_data_complex[j].imag(0);
        }
        fft.transform(window_data_complex, fd);
        double *fd_real_unproc = new double[WindowSize];
        double *fd_imag_unproc = new double[WindowSize];
        double *fd_real_proc = new double[WindowSize];
        double *fd_imag_proc = new double[WindowSize];
        for (size_t j = 0; j < WindowSize; ++j) {
            fd_real_unproc[j] = fd[j].real();/*
            if (i == 500) {
                printf("%f\n", fd_real_unproc[j]);
            }*/
            fd_imag_unproc[j] = fd[j].imag();
        }
        size_t new_length = ratio < 1 ? WindowSize * ratio : WindowSize;
        resample_linear(fd_real_unproc, fd_real_proc, ratio, WindowSize, new_length);
        resample_linear(fd_imag_unproc, fd_imag_proc, ratio, WindowSize, new_length);
        for (size_t j = 0; j < WindowSize; ++j) {
            fd[j].real(fd_real_proc[j]);
            fd[j].imag(fd_imag_proc[j]);
        }
        fft_inverse.transform(fd, inversed);
        for (size_t j = 0; j < WindowSize; ++j) {
            (*out)[i*WindowSize+j] = inversed[j].real() / WindowSize;
            double x = (*out)[i*WindowSize+j]-window_data[j];
        }
        if (need_delete) {
            delete window_data;
        }
        delete window_data_complex;
        delete inversed;
        delete fd;
        delete fd_real_unproc;
        delete fd_imag_unproc;
        delete fd_real_proc;
        delete fd_imag_proc;
    }
    return new_length;
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
    size_t new_length;
    double ratio = 1.1;
    for (size_t i = 0; i < mdata.size(); ++i) {
        double *old_data = mdata[i];
        double *new_data = nullptr;
        new_length = shift_pitch(old_data, &new_data, ratio, length);
        delete old_data;
        mdata[i] = new_data;
    }
    length = new_length;
    /*
    */
    /*
    size_t new_length = ceil(length * ratio);
    for (size_t i = 0; i < mdata.size(); ++i) {
        double *old_data = mdata[i];
        double *new_data = new double[new_length];
        resample_linear(old_data, new_data, ratio, length, new_length);
        delete old_data;
        mdata[i] = new_data;
    }
    */
    size_t mlength = length * mdata.size();
    join_channels(mdata, &data, length);
    write_wav("2.wav", mdata.size(), 44100, data, mlength);
    delete data;
    clear_data(mdata);
    return 0;
}