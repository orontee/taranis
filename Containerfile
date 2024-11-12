# -*- mode: dockerfile-ts; -*-

FROM debian:stable AS build

RUN apt-get update -y && apt-get upgrade -y && \
    DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
      build-essential \
      ca-certificates \
      clang \
      git \
      libtinfo5 \
      meson \
      python3-jinja2 \
      wget \
      zip && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

COPY . /src

WORKDIR /src

ARG VERSION
RUN [ -n "${VERSION}" ]

RUN git checkout ${VERSION} && \
    git submodule init && \
    git submodule update

RUN ./scripts/install_sdk.sh
RUN ./scripts/generate_cross_compilation_conf.sh

RUN source ./SDK_6.8.0/env_set.sh && \
    cd 3rd-parties && \
    wget https://ftp.gnu.org/gnu/gsl/gsl-2.7.1.tar.gz && \
    sha256sum -c gsl-2.7.1.tar.gz.sha256 && \
    tar -xzf gsl-2.7.1.tar.gz && \
    cd gsl-2.7.1 && \
    ./configure --prefix=$PWD/../../SDK_6.8.0/SDK-B288/usr/arm-obreey-linux-gnueabi/sysroot \
                --host=arm-obreey-linux-gnueabi \
                --build=x86_64-pc-linux-gnu \
                --target=arm-obreey-linux-gnueabi && \
    make -j$(nproc) && \
    make install

ARG MESON_ARGS
RUN meson setup builddir . --cross-file crossfile_arm.ini ${MESON_ARGS} && \
    DESTDIR=artifact meson install -C builddir && \
    meson compile -C builddir installer && \
    meson compile -C builddir archive && \
    meson compile -C builddir sha
