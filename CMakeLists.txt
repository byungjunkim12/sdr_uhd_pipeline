cmake_minimum_required(VERSION 3.15)
project(sdr_uhd_pipeline)
set(CMAKE_CXX_STANDARD 17)
find_package(UHD REQUIRED)

add_executable(sdr_uhd_pipeline
    src/main.cpp
    src/producer.cpp
    src/consumer_usrp.cpp
    src/producer_usrp.cpp
    src/consumer.cpp
)
target_include_directories(sdr_uhd_pipeline PRIVATE include ${UHD_INCLUDE_DIRS})
target_link_libraries(sdr_uhd_pipeline ${UHD_LIBRARIES})

