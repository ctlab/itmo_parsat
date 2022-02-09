FROM ubuntu:20.04

ARG user
ARG group
ARG uid
ARG gid
ARG DEBIAN_FRONTEND=nointeractive

RUN groupadd --gid ${gid} ${group} && useradd --uid ${uid} --gid ${gid} --shell /bin/bash --create-home ${user}

# Install required packages
RUN apt update && apt upgrade -y && \
    apt install -y apt-transport-https curl gnupg && \
    curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel.gpg && \
    mv bazel.gpg /etc/apt/trusted.gpg.d/ && \
    echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list && \
    apt update && apt install -y vim bazel git libpq5 libpq-dev libpqxx-dev libpqxx-6.4 postgresql-12 clang-format doxygen

USER ${user}
WORKDIR /home/${user}
