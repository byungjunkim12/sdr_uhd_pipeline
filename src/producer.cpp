#include <string>
#include <complex>
#include <fstream>
#include <iostream>
#include "queue.h"

void producer(IQQueue<std::complex<float>>& queue, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "failed to open: " << filename << "\n";
        queue.set_done();
        return;
    }
    std::complex<float> sample;
    while (file.read(reinterpret_cast<char*>(&sample), sizeof(sample))) {
        queue.push(sample);
    }
}
