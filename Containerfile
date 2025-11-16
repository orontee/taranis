# -*- mode: dockerfile-ts; -*-

FROM ubuntu:22.04 AS build

RUN apt-get update -y && apt-get upgrade -y && \
    DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
      build-essential \
      ca-certificates \
      clang \
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

RUN mkdir /src
WORKDIR /src

COPY ./scripts/install_sdk.sh ./scripts/
COPY ./SDK-B288-6.8.7z.sha256 .
RUN ./scripts/install_sdk.sh
RUN rm ./SDK-B288-6.8.7z

ENV PKG_CONFIG_PATH=/src/SDK_6.8.0/SDK-B288/usr/arm-obreey-linux-gnueabi/sysroot/libcurl-nspr/usr/lib/pkgconfig/:/src/SDK_6.8.0/SDK-B288/usr/arm-obreey-linux-gnueabi/sysroot/usr/lib/pkgconfig:/src/SDK_6.8.0/SDK-B288/usr/arm-obreey-linux-gnueabi/sysroot/lib/pkgconfig
