FROM ubuntu:20.04

ARG user
ARG DEBIAN_FRONTEND=nointeractive

WORKDIR /tmp

# Apt packages
RUN apt update && apt upgrade -y && \
    apt install -y wget gcc-10 g++-10 gcc g++ make libssl-dev libgtest-dev \
    libgoogle-glog-dev git libopenmpi-dev libpqxx-dev zlib1g-dev vim doxygen clang-format libtbb-dev

# CMake
RUN wget https://github.com/Kitware/CMake/releases/download/v3.23.0-rc1/cmake-3.23.0-rc1.tar.gz && \
    tar xvf cmake-3.23.0-rc1.tar.gz --no-same-owner && cd cmake-3.23.0-rc1 && \
    ./configure && make -j $(nproc) && make install

# Boost 1.76
RUN wget https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.gz && \
    tar xf boost_1_76_0.tar.gz --no-same-owner && cd boost_1_76_0 && \
    ./bootstrap.sh && ./b2 install

# GLog
RUN git clone https://github.com/google/glog.git && \
    cd glog && \
    cmake -S . -B build -G "Unix Makefiles" && \
    cmake --build build && \
    cmake --build build --target install

# Protobuf
RUN wget https://github.com/protocolbuffers/protobuf/releases/download/v3.14.0/protobuf-all-3.14.0.tar.gz && \
    tar xvf protobuf-all-3.14.0.tar.gz --no-same-owner && cd protobuf-3.14.0 && \
    ./configure && make -j $(nproc) && make install && ldconfig

WORKDIR /home/${user}
