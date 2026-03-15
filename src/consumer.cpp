#include <complex>
#include <iostream>
#include <fstream>
#include "queue.h"

void consumer(IQQueue<std::complex<float>>& queue, const std::string& output_filename, int queue_id) {
    size_t count = 0;
    std::complex<float> sample;
    std::ofstream outfile(output_filename, std::ios::binary);
    while (queue.pop(sample)){
        outfile.write(reinterpret_cast<const char*>(&sample), sizeof(sample));
        if (count % 1000 == 0) std::cout << "[queue" << queue_id << "] sample[" << count << "]: " << sample << "\n";
        count++;
    }
}
