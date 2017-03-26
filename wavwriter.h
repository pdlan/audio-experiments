#ifndef WAVWRITER_H
#define WAVWRITER_H
#include <fstream>
#include "util.h"

void write_wav(const std::string &filename, const WavInfo &info, double *data);
#endif