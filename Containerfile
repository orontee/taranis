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

RUN ./scripts/generate_cross_compilation_conf.sh

RUN mkdir 3rd-parties && \
    cd 3rd-parties && \
    wget https://ftp.gnu.org/gnu/gsl/gsl-2.7.1.tar.gz && \
    tar -xzf gsl-2.7.1.tar.gz && \
    cd gsl-2.7.1 && \
    CROSS=arm-obreey-linux-gnueabi && \
    ./configure --prefix=$PWD/../../SDK_6.3.0/SDK-B288/usr/$CROSS/sysroot \
               --host=$CROSS \
               --build=x86_64-pc-linux-gnu \
               --target=$CROSS \
      CC=$PWD/../../SDK_6.3.0/SDK-B288/usr/bin/$CROSS-clang \
      CXX=$PWD/../../SDK_6.3.0/SDK-B288/usr/bin/$CROSS-clang++ \
      AR=$PWD/../../SDK_6.3.0/SDK-B288/usr/bin/$CROSS-ar \
      STRIP=$PWD/../../SDK_6.3.0/SDK-B288/usr/bin/$CROSS-strip \
      RANLIB=$PWD/../../SDK_6.3.0/SDK-B288/usr/bin/$CROSS-ranlib \
      PKGCONFIG=$PWD/../../SDK_6.3.0/SDK-B288/usr/bin/pkg-config \
      CFLAGS="-march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp" && \
    make -j4 && \
    make install

ARG MESON_ARGS
RUN meson setup builddir . --cross-file crossfile_arm.ini ${MESON_ARGS} && \
    ninja -C builddir && \
    DESTDIR=artifact ninja -C builddir install
RUN cd builddir; zip -r artifact.zip artifact/
