#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/types/tune_request.hpp>
#include <uhd/stream.hpp>
#include <complex>
#include <iostream>
#include <vector>
#include "queue.h"

void consumer_usrp(IQQueue<std::complex<float>>& queue,
                    const std::string& usrp_addr,
                    double freq,
                    double rate,
                    double gain) {
    
    // Initialize USRP connection
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make("addr=" + usrp_addr);
    
    // TX configuration
    usrp->set_tx_rate(rate);
    usrp->set_tx_freq(uhd::tune_request_t(freq));
    usrp->set_normalized_tx_gain(gain, 0);
    usrp->set_tx_antenna("TX/RX", 0);

    // Generate tx_streamer
    uhd::stream_args_t stream_args("fc32");
    stream_args.channels = {0};
    uhd::tx_streamer::sptr tx_stream = usrp->get_tx_stream(stream_args);
    
    // send() loop
    uhd::tx_metadata_t metadata;
    metadata.start_of_burst = true;
    metadata.end_of_burst   = false;
    
    std::complex<float> sample;
    std::vector<std::complex<float>> buf;

    while (queue.pop(sample)) {
        buf.push_back(sample);
        if (buf.size() == 1024) {
            tx_stream->send(&buf[0], buf.size(), metadata);
            metadata.start_of_burst = false;
            buf.clear();
        }
        
    }
    
    if (!buf.empty()) {
        metadata.end_of_burst = true;
        tx_stream->send(&buf[0], buf.size(), metadata);
    }
}
