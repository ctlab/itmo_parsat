################### Build itmo-parsat solver
FROM satcomp-common-base-image
USER root

WORKDIR /tmp

# Apt packages
RUN apt update && apt upgrade -y && \
    apt install -y wget gcc-10 g++-10 gcc g++ make libssl-dev libgtest-dev libunwind-dev lzma-dev \
    libgoogle-glog-dev git libpqxx-dev zlib1g-dev vim doxygen clang-format libtbb-dev

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

# Google benchmark
RUN git clone https://github.com/google/benchmark.git && \
    cd benchmark && \
    cmake -E make_directory "build" && \
    cmake -E chdir "build" cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release ../ && \
    cmake --build "build" --config Release && \
    cmake --build "build" --config Release --target install

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt install -y cmake build-essential zlib1g-dev libopenmpi-dev
    wget unzip build-essential zlib1g-dev cmake python3 build-essential gfortran wget curl

WORKDIR /
RUN git clone https://github.com/dzhiblavi/itmo-parsat
WORKDIR /itmo-parsat
RUN ./setup.sh && CC=gcc-10 CXX=g++-10 ./ips --clean -g DEV_FAST -b

