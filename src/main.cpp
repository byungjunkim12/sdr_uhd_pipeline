#include <thread>
#include <complex>
#include <string>
#include <vector>
#include <chrono>
#include "queue.h"

void producer(IQQueue<std::complex<float>>& queue, const std::string& filename);
void consumer_usrp(IQQueue<std::complex<float>>& queue,
                const std::string& usrp_addr,
                double freq,
                double rate,
                double gain);

void producer_usrp(IQQueue<std::complex<float>>& queue,
        const std::string& usrp_addr,
        double freq,
        double rate,
        double gain,
        size_t num_samples);
void consumer(IQQueue<std::complex<float>>& queue, const std::string& output_filename, int queue_id);

int main(void) {
    std::string input_dataset_path = "/root/sdr_uhd_pipeline/data/USRP_input/";
    std::string output_dataset_path = "/root/sdr_uhd_pipeline/data/USRP_output/";

    std::vector<std::string> usrp_addrs_tx = {
        "10.10.24.1",
        "10.10.24.3"
    };
    std::vector<std::string> usrp_addrs_rx = {
        "10.10.24.18",
        "10.10.24.17"};

    double carrier_freq     = 5.0e9;
    double sample_rate      = 1.0e6;
    double tx_gain          = 1.0;
    double rx_gain          = 0.8;
    size_t num_samples_file = 100000;

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

    std::vector<IQQueue<std::complex<float>>> queues_tx;
    std::vector<IQQueue<std::complex<float>>> queues_rx;
    for (int i = 0; i < MIMO_size; i++) {
        queues_tx.emplace_back(num_samples_file);
        queues_rx.emplace_back(num_samples_file);
    }
    
    std::vector<std::thread> producers, consumers_usrp;
    std::vector<std::thread> producers_usrp, consumers;
    for (int i = 0; i < MIMO_size; i++) {
        producers.emplace_back(producer, std::ref(queues_tx[i]), input_files[i]);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    for (int i = 0; i < MIMO_size; i++) {
        consumers_usrp.emplace_back(consumer_usrp,
            std::ref(queues_tx[i]),
            usrp_addrs_tx[i],
            carrier_freq,
            sample_rate,
            tx_gain);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for (int i = 0; i < MIMO_size; i++) {    
        producers_usrp.emplace_back(producer_usrp,
            std::ref(queues_rx[i]),
            usrp_addrs_rx[i],
            carrier_freq,
            sample_rate,
            rx_gain,
            num_samples_file);
        consumers.emplace_back(consumer, std::ref(queues_rx[i]), output_files[i], i);
    }
    
    for (auto& producer_usrp : producers_usrp) producer_usrp.join();
    for (auto& queue_tx : queues_tx) queue_tx.set_done();
    for (auto& consumer_usrp : consumers_usrp) consumer_usrp.join();
    for (auto& producer : producers) producer.join();
    for (auto& consumer : consumers) consumer.join();
}
