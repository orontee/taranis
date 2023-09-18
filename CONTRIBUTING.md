# Contributing to Taranis

## Dependencies

### PocketBook SDK

The [PocketBook SDK](https://github.com/pocketbook/SDK_6.3.0) is
hosted on Github and managed as a Git submodule.

### GNU Scientific Library

The [GNU Scientific Library
dependency](https://www.gnu.org/software/gsl/) is used to interpolate
hourly temperatures and draw a smooth curve. It must be cross-compiled
and installed manually, see instructions below.

## Build

### Using a container

To build the application *for a given version*, one can build a Docker
image and export the ELF file from that image::
```sh
  VERSION=v$(meson version | grep 'Project version: ' | cut -d':' -f 2 | tr -d '[:space:]')
  buildah bud -t taranis-build:$VERSION --build-arg VERSION=${VERSION} .
  podman run --rm -v ${PWD}:/opt/taranis \
             taranis-build:$VERSION bash -c "cp builddir/taranis.app /opt/taranis"
```

### Cross-compile on a Debian host

Read the [Containerfile](./Containerfile) to fix potential missing
dependencies.

1. First, clone the source repository and populate Git submodules:
   ```sh
   git submodule init
   git submodule update
   ```

   This will populate the `SDK_6.3.0` directory with the PocketBook
   SDK.

2. Update SDK paths and generate cross compilation configuration (It
   will generate the file `crossfile_arm.ini` used to build):
   ```sh
   ./scripts/generate_cross_compilation_conf.sh
   ```

3. Download, build and install source code of the [GNU Scientific
   Library dependency](https://www.gnu.org/software/gsl/):
   ```sh
   mkdir 3rd-parties
   pushd 3rd-parties
   wget https://ftp.gnu.org/gnu/gsl/gsl-2.7.1.tar.gz
   tar -xzf gsl-2.7.1.tar.gz
   pushd gsl-2.7.1
   CROSS=arm-obreey-linux-gnueabi
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
      CFLAGS="-march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp"
   make -j4
   make install
   popd
   popd
   ```

4. Finally build the application:
   ```sh
   meson setup builddir . --cross-file crossfile_arm.ini
   pushd builddir && meson compile; popd
   ```

## Tooling

Format sources:
```sh
ninja -C builddir clang-format
```

Static analysis:
```sh
SCANBUILD=./SDK_6.3.0/SDK-B288/usr/arm-obreey-linux-gnueabi/bin/scan-build ninja -C builddir
```

## Screenshots

Screenshots can be performed through the task menu of an e-reader, and conversion can be done
using [ImageMagick convert script](https://www.imagemagick.org/script/convert.php); For example:
```
convert /media/matthias/Vivlio/screens/scr0002.bmp \
        -scale 20% -bordercolor pink -border 5 \
        docs/screenshot-hourly-forecast.jpg
```

## References

* https://github.com/pocketbook-free/InkViewDoc
* https://github.com/JuanJakobo/Pocketbook-Read-offline
* https://github.com/SteffenBauer/PocketPuzzles
* https://github.com/pmartin/pocketbook-demo
