FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y clang-format && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

#test workflow trigger 5