#include <iostream>
#include <string>
#include <cmath>
#include <functional>
#include <cstring>
#include <vector>
#include <deque>
#include <algorithm>
#include <fftw3.h>
#include "shift_pitch.h"
#include "resample.h"
#include "util.h"

size_t shift_pitch_one(const double *in, double **out, double ratio,
                       const WindowFunction &window, size_t length) {
    size_t window_size = window.size();
    *out = new double[window_size];
    double *window_data = new double[window_size];
    fftw_complex *fd = new fftw_complex[window_size];
    for (size_t i = 0;
         i < ((length < window_size) ? length : window_size);
         ++i) {
        window_data[i] = in[i] * window.window(i);
    }
    fftw_plan plan = fftw_plan_dft_r2c_1d(window_size, window_data, fd, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    size_t fd_length = window_size / 2 + 1;
    double *fd_real_unproc = new double[fd_length];
    double *fd_imag_unproc = new double[fd_length];
    double *fd_real_proc = new double[fd_length];
    double *fd_imag_proc = new double[fd_length];
    for (size_t i = 0; i < window_size / 2 + 1; ++i) {
        if (ratio >= 1 || i < window_size) {
            fd_real_unproc[i] = fd[i][0];
            fd_imag_unproc[i] = fd[i][1];
        }
    }
    size_t new_length = ratio < 1 ? fd_length * ratio : fd_length;
    resample_linear(fd_real_unproc, fd_real_proc, ratio, fd_length, new_length);
    resample_linear(fd_imag_unproc, fd_imag_proc, ratio, fd_length, new_length);
    for (size_t i = 0; i < window_size / 2 + 1; ++i) {
        fd[i][0] = fd_real_proc[i];
        fd[i][1] = fd_imag_proc[i];
    }
    fftw_plan plan_i = fftw_plan_dft_c2r_1d(window_size, fd, window_data, FFTW_ESTIMATE);
    fftw_execute(plan_i);
    fftw_destroy_plan(plan_i);
    for (size_t i = 0; i < window_size; ++i) {
        (*out)[i] = window_data[i] / window_size;
    }
    delete window_data;
    delete[] fd;
    delete fd_real_unproc;
    delete fd_imag_unproc;
    delete fd_real_proc;
    delete fd_imag_proc;
    return window_size;
}


size_t shift_pitch(const double *in, double **out, double ratio,
                   const RectWindow &window, size_t length) {
    using namespace std;
    const int OverlapNumber = 2;
    const int ValidChunk = 2;
    deque<double *> data;
    size_t window_size = window.size();
    size_t chunk_size = window_size / OverlapNumber;
    if (window_size / chunk_size == 0) {
        return shift_pitch_one(in, out, ratio, window, length);
    }
    size_t window_number = ceil(length / double(chunk_size));
    size_t new_length = window_number * chunk_size;
    *out = new double[new_length];
    for (size_t i = 0; i < window_number; ++i) {
        double *proceeded = nullptr;
        if ((window_number - i) <= OverlapNumber) {
            size_t length_left = length - i * chunk_size;
            shift_pitch_one(in + i * chunk_size, &proceeded, ratio, window,
                            length_left);
        } else {
            shift_pitch_one(in + i * chunk_size, &proceeded, ratio, window,
                            window_size);
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
            if (data.size() < OverlapNumber) {
                for (size_t k = 0; k < data.size(); ++k) {
                    value += data[k][j+(data.size()-k-1)*chunk_size];
                }
                (*out)[i*chunk_size+j] = value / OverlapNumber;
            } else {
                for (size_t k = (OverlapNumber - ValidChunk) / 2;
                     k < data.size() - (OverlapNumber - ValidChunk) / 2; ++k) {
                    value += data[k][j+(data.size()-k-1)*chunk_size];
                }
                (*out)[i*chunk_size+j] = value / ValidChunk;
            }
        }
    }
    for (deque<double*>::iterator i = data.begin(); i != data.end(); ++i) {
        delete *i;
    }
    return new_length;
}


size_t shift_pitch(const double *in, double **out, double ratio,
                   const WindowFunction &window, size_t length) {
    using namespace std;
    size_t window_size = window.size();
    size_t chunk_size = window.chunk_size();
    size_t window_number = ceil(length / double(chunk_size));
    size_t new_length = (window_number - 1) * chunk_size + window_size;
    double *data = new double[new_length];
    copy(in, in + length, data);
    *out = new double[new_length];
    for (size_t i = 0; i < window_number; ++i) {
        double *buffer = nullptr;
        shift_pitch_one(data + i * chunk_size, &buffer, ratio,
                        window, window_size);
        for (size_t j = 0; j < window_size; ++j) {
            (*out)[i*chunk_size+j] += buffer[j];
        }
        delete buffer;
    }
    delete data;
    return new_length;
}
