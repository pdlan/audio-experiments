#include <iostream>
#include <string>
#include <cmath>
#include <functional>
#include <cstring>
#include <vector>
#include <algorithm>
#include "kissfft.hh"
#include "shift_pitch.h"
#include "resample.h"
#include "util.h"

size_t shift_pitch_(const double *in, double **out, double ratio,
                   const WindowFunction &window, size_t length) {
    using namespace std;
    size_t WindowSize = window.size();
    size_t window_number = ceil(double(length) / WindowSize);
    size_t new_length = window_number * WindowSize;
    *out = new double[new_length];
    kissfft<double> fft(WindowSize, false);
    kissfft<double> fft_inverse(WindowSize, true);
    typedef kissfft<double>::cpx_t ComplexType;
    for (size_t i = 0; i < window_number; ++i) {
        double *window_data = new double[WindowSize];
        if (i < (window_number - 1)) {
            for (size_t j = 0; j < WindowSize; ++j) {
                window_data[j] = in[i*WindowSize+j] * window.window(j);
            }
        } else {
            window_data = new double[WindowSize];
            size_t length_left = length - WindowSize * i;
            for (size_t j = 0; j < length_left; ++j) {
                window_data[j] = in[i*WindowSize+j] * window.window(j);
            }
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
            fd_real_unproc[j] = fd[j].real();
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

size_t shift_pitch(const double *in, double **out, double ratio,
                   const WindowFunction &window, size_t length) {
    using namespace std;
    size_t WindowSize = window.size();
    size_t window_number = ceil(double(length) / WindowSize);
    size_t new_length = window_number * WindowSize;
    double *out1 = nullptr;
    double *out2 = nullptr;
    double *out3 = nullptr;
    double *out4 = nullptr;
    shift_pitch_(in, &out1, ratio, window, length);
    shift_pitch_(in + WindowSize / 4, &out2, ratio, window, length - WindowSize / 4);
    shift_pitch_(in + WindowSize * 2/ 4, &out2, ratio, window, length - WindowSize *2 / 4);
    shift_pitch_(in + WindowSize * 3/ 4, &out2, ratio, window, length - WindowSize *3 / 4);
    *out = new double[new_length];
    for (size_t i = 0; i < window_number; ++i) {
        if (i == 0) {
            for (size_t j = 0; j <  WindowSize; ++j) {
                (*out)[i*WindowSize+j] = out1[i*WindowSize+j];
            }
        } else {
            for (size_t j = 0; j <  WindowSize; ++j) {
                (*out)[i*WindowSize+j] =
                    (out1[i*WindowSize+j] +
                    out2[i*WindowSize+j-WindowSize/4] +
                    out2[i*WindowSize+j-WindowSize*2/4] +
                    out2[i*WindowSize+j-WindowSize*3/4]) / 4;
            }
        }
    }
    delete out1;
    delete out2;
    delete out3;
    delete out4;
    return new_length;
}