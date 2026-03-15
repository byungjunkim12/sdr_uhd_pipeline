#include <thread>
#include <complex>
#include <string>
#include <vector>
#include "queue.h"

void producer(IQQueue<std::complex<float>>& queue, const std::string& filename);
void consumer(IQQueue<std::complex<float>>& queue, const std::string& output_filename, int queue_id);

int main(void) {
    std::string input_dataset_path = "/root/sdr_uhd_pipeline/data/USRP_input/";
    std::string output_dataset_path = "/root/sdr_uhd_pipeline/data/USRP_output/";

    int MIMO_size   = 2;
    int modOrder    = 1;
    int SEED        = 0;

    std::vector<std::string> input_files(MIMO_size);
    std::vector<std::string> output_files(MIMO_size);
    for (int i = 0; i < MIMO_size; i++) {
        input_files[i] = input_dataset_path +
            "wlan_nMIMO_" + std::to_string(MIMO_size) +
            "_modOrder_" + std::to_string(modOrder) +
            "_SEED_" + std::to_string(SEED) +
            "_TX_" + std::to_string(i) + ".iq";
        
        output_files[i] = output_dataset_path + "output_RX_" + std::to_string(i) + ".iq"; 
    }

    std::vector<IQQueue<std::complex<float>>> queues;
    for (int i = 0; i < MIMO_size; i++) 
        queues.emplace_back(100);
    
    std::vector<std::thread> producers, consumers;
    for (int i = 0; i < MIMO_size; i++) {
        producers.emplace_back(producer, std::ref(queues[i]), input_files[i]);
        consumers.emplace_back(consumer, std::ref(queues[i]), output_files[i], i);
    }
    
    for (auto& producer : producers) producer.join();
    for (auto& queue : queues) queue.set_done();
    for (auto& consumer : consumers) consumer.join();
}
