#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <vector>
#include <functional>
#include <cmath>

const double Pi = 3.14159265358979323846;
typedef std::vector<double *> MultiChannelData;
size_t load_wav(const std::string &filename, MultiChannelData &data);
size_t join_channels(const MultiChannelData &in, double **out, size_t length);
void clear_data(MultiChannelData &data);

class WindowFunction {
    public:
        virtual size_t size() const = 0;
        virtual double window(size_t x) const = 0;
};

class RectWindow : public WindowFunction {
    public:
        RectWindow(size_t size) : _size(size) {}
        size_t size() const {return this->_size;}
        double window(size_t x) const {return 1.0;}
    private:
        size_t _size;
};

class HannWindow : public WindowFunction {
    public:
        HannWindow(size_t size) : _size(size) {
            for (size_t i = 0; i < size; ++i) {
                value.push_back(0.5 * (1 - cos(2 * Pi * i / (_size - 1))));
            }
        }
        size_t size() const {return this->_size;}
        double window(size_t x) const {
            return value[x];
        }
    private:
        size_t _size;
        std::vector<double> value;
};
#endif