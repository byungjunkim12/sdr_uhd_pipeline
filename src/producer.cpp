#include <string>
#include <complex>
#include <fstream>
#include <iostream>
#include "queue.h"

void producer(IQQueue<std::complex<float>>& queue, const std::string& filename) {
    while (true) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "failed to open: " << filename << "\n";
            break;
        }
        std::complex<float> sample;
        while (file.read(reinterpret_cast<char*>(&sample), sizeof(sample))){ 
            if (queue.is_done()) return;
            queue.push(sample);
        }
    }
}
