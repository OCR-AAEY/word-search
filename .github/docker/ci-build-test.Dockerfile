FROM ubuntu:24.04

# Prevent tzdata prompts
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    libgtk-3-dev \
    libcriterion-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*