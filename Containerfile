# -*- mode: dockerfile-ts; -*-

FROM ubuntu:22.04 AS build

RUN apt-get update -y && apt-get upgrade -y && \
    DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
      build-essential \
      ca-certificates \
      clang \
      clangd \
      clang-format \
      clang-tidy \
      cmake \
      file \
      git \
      libtinfo5 \
      pkg-config \
      python3-jinja2 \
      wget \
      zip \
      7zip && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

ARG DEVICE_FAMILY=B288

ENV SDK_PARENT_PATH="/opt/SDK-6.8"
WORKDIR ${SDK_PARENT_PATH}

COPY ./scripts/install_sdk.sh .
COPY ./SDK-${DEVICE_FAMILY}-6.8.7z.sha256 .
RUN ./install_sdk.sh --family ${DEVICE_FAMILY} --path ${SDK_PARENT_PATH} && \
    rm install_sdk.sh \
       SDK-${DEVICE_FAMILY}-6.8.7z \
       SDK-${DEVICE_FAMILY}-6.8.7z.sha256

WORKDIR /src
