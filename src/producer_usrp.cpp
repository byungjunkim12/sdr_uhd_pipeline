#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/types/tune_request.hpp>
#include <complex>
#include <iostream>
#include "queue.h"

void producer_usrp(IQQueue<std::complex<float>>& queue,
        const std::string& usrp_addr,
        double freq,
        double rate,
        double gain,
        size_t num_samples) {

    // Initialize USRP connection
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make("addr=" + usrp_addr);
    
    // RX configuration
    usrp->set_rx_rate(rate);
    usrp->set_rx_freq(uhd::tune_request_t(freq));
    usrp->set_rx_gain(gain);
    
    // Generate rx_streamer
    uhd::stream_args_t stream_args("fc32");
    uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);

    // Initiate streaming
    uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
    stream_cmd.stream_now = true;
    rx_stream->issue_stream_cmd(stream_cmd);

    // recv() loop
    uhd::rx_metadata_t metadata;
    std::vector<std::complex<float>> buf(1024);
    size_t total = 0;

    while (total < num_samples) {
        size_t n = rx_stream->recv(&buf[0], buf.size(), metadata);
        if (metadata.error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW) {
            std::cerr << "Overflow!\n";
            continue;
        }
        if (metadata.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
            std::cerr << "recv error: " << metadata.strerror() << "\n";
            break;
        }

        for (size_t i = 0; i < n; i++)
            queue.push(buf[i]);

        total += n;
    }

    stream_cmd = uhd::stream_cmd_t(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
    rx_stream->issue_stream_cmd(stream_cmd);
    queue.set_done();    
}
