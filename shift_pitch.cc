#include <iostream>
#include <string>
#include <cmath>
#include <functional>
#include <cstring>
#include <vector>
#include <deque>
#include <algorithm>
#include "kissfft.hh"
#include "shift_pitch.h"
#include "resample.h"
#include "util.h"

size_t shift_pitch_one(const double *in, double **out, double ratio,
                       const WindowFunction &window, size_t length,
                       const kissfft<double> &fft,
                       const kissfft<double> &ifft) {
    typedef std::complex<double> ComplexType;
    size_t window_size = window.size();
    *out = new double[window_size];
    double *window_data = new double[window_size];
    for (size_t i = 0;
         i < ((length < window_size) ? length : window_size);
         ++i) {
        window_data[i] = in[i] * window.window(i);
    }
    ComplexType *window_data_complex = new ComplexType[window_size];
    ComplexType *fd = new ComplexType[window_size];
    ComplexType *inversed = new ComplexType[window_size];
    for (size_t i = 0; i < window_size; ++i) {
        window_data_complex[i].real(window_data[i]);
        window_data_complex[i].imag(0);
    }
    fft.transform(window_data_complex, fd);
    double *fd_real_unproc = new double[window_size];
    double *fd_imag_unproc = new double[window_size];
    double *fd_real_proc = new double[window_size];
    double *fd_imag_proc = new double[window_size];
    for (size_t i = 0; i < window_size; ++i) {
        fd_real_unproc[i] = fd[i].real();
        fd_imag_unproc[i] = fd[i].imag();
    }
    size_t new_length = ratio < 1 ? window_size * ratio : window_size;
    resample_linear(fd_real_unproc, fd_real_proc, ratio, window_size, new_length);
    resample_linear(fd_imag_unproc, fd_imag_proc, ratio, window_size, new_length);
    for (size_t i = 0; i < window_size; ++i) {
        fd[i].real(fd_real_proc[i]);
        fd[i].imag(fd_imag_proc[i]);
    }
    ifft.transform(fd, inversed);
    for (size_t i = 0; i < window_size; ++i) {
        (*out)[i] = inversed[i].real() / window_size;
    }
    delete window_data_complex;
    delete inversed;
    delete fd;
    delete fd_real_unproc;
    delete fd_imag_unproc;
    delete fd_real_proc;
    delete fd_imag_proc;
    delete window_data;
    return window_size;
}

size_t shift_pitch(const double *in, double **out, double ratio,
                   const WindowFunction &window, size_t length) {
    using namespace std;
    const int OverlapNumber = 2;
    deque<double *> data;
    size_t window_size = window.size();
    kissfft<double> fft(window_size, false);
    kissfft<double> ifft(window_size, true);
    size_t chunk_size = window_size / OverlapNumber;
    if (window_size / chunk_size == 0) {
        return shift_pitch_one(in, out, ratio, window, length, fft, ifft);
    } else if (window_size / chunk_size == 1) {
        *out = nullptr;
        return 0;
    }
    size_t window_number = ceil(length / double(chunk_size));
    size_t new_length = window_number * chunk_size;
    *out = new double[new_length];
    for (size_t i = 0; i < window_number; ++i) {
        double *proceeded = nullptr;
        if ((window_number - i) <= OverlapNumber) {
            size_t length_left = length - i * chunk_size;
            shift_pitch_one(in + i * chunk_size, &proceeded, ratio, window,
                            length_left, fft, ifft);
        } else {
            shift_pitch_one(in + i * chunk_size, &proceeded, ratio, window,
                            window_size, fft, ifft);
        }
        if (data.size() < OverlapNumber) {
            data.push_back(proceeded);
        } else {
            double *first_chunk = data.front();
            delete first_chunk;
            data.pop_front();
            data.push_back(proceeded);
        }
        for (size_t j = 0; j < chunk_size; ++j) {
            double value = 0.0;
            for (size_t k = 0; k < data.size(); ++k) {
                value += data[data.size()-k-1][j+k*chunk_size];
            }
            (*out)[i*chunk_size+j] = value / OverlapNumber;
        }
    }
    return new_length;
}