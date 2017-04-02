#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <vector>
#include <functional>
#include <cmath>

const double Pi = 3.14159265358979323846;
typedef std::vector<double *> MultiChannelData;
struct WavInfo {
    size_t length;
    unsigned int sample_rate;
    unsigned short channels;
    unsigned short bits_per_sample;
    unsigned short bytes_per_sample;
};
WavInfo load_wav(const std::string &filename, MultiChannelData &data);
size_t join_channels(const MultiChannelData &in, double **out, size_t length);
void clear_data(MultiChannelData &data);

class WindowFunction {
    public:
        virtual size_t size() const = 0;
        virtual double window(size_t x) const = 0;
        virtual size_t chunk_size() const = 0;
        virtual ~WindowFunction() {}
};

class RectWindow : public WindowFunction {
    public:
        RectWindow(size_t size) : _size(size) {
            _chunk_size = _size;
        }
        size_t size() const {return _size;}
        double window(size_t x) const {return 1.0;}
        size_t chunk_size() const {return _chunk_size;}
    private:
        size_t _size;
        size_t _chunk_size;
};

class HannWindow : public WindowFunction {
    public:
        HannWindow(size_t size) : _size(size) {
            for (size_t i = 0; i < size; ++i) {
                value.push_back(0.5 * (1 - cos(2 * Pi * i / (_size - 1))));
            }
            _chunk_size = _size / 2;
        }
        size_t size() const {return _size;}
        double window(size_t x) const {
            return value[x];
        }
        size_t chunk_size() const {return _chunk_size;}
    private:
        size_t _size;
        size_t _chunk_size;
        std::vector<double> value;
};

class BlackmanWindow : public WindowFunction {
    public:
        BlackmanWindow(size_t size) : _size(size) {
            for (size_t i = 0; i < size; ++i) {
                const double a0 = 7938.0 / 18608.0;
                const double a1 = 9240.0 / 18608.0;
                const double a2 = 1430.0 / 18608.0;
                value.push_back(a0 - a1 * cos(2 * Pi * i / (_size - 1)) +
                                a2 * cos(4 * Pi * i / (_size - 1)));
            }
            _chunk_size = _size / 2;
        }
        size_t size() const {return this->_size;}
        double window(size_t x) const {
            return value[x];
        }
        size_t chunk_size() const {return _chunk_size;}
    private:
        size_t _size;
        size_t _chunk_size;
        std::vector<double> value;
};
#endif