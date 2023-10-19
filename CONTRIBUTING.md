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
image and export an archive containing the ELF file from that image. 
For example, for a debug build of the most recent tagged version:
```sh
VERSION=$(git tag --list 'v*' | sort --version-sort --reverse | head -n 1)
buildah bud -t taranis-build:${VERSION} \
            --build-arg="VERSION=${VERSION}" \
            --build-arg="MESON_ARGS=--buildtype=debug" .
podman run --rm -v ${PWD}:/opt/taranis \
           taranis-build:$VERSION bash -c "cp builddir/artifact.zip /opt/artifact.zip"
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
   meson setup builddir . \
         --cross-file crossfile_arm.ini \
         --buildtype=debug
   DESTDIR=artifact ninja -C builddir install
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

## Translations

Translations are stored in [PO files](./po) which are textual,
editable files]. They are generated using `xgettext`.

To update translation files:
```sh
DESTDIR=artifact ninja -C builddir taranis-update-po
```

A custom target generates a C++ file from all PO files, see
[generate_l10n_cc.py](./scripts/generate_l10n_cc.py).

## Screenshots

Screenshots can be performed through the task menu of an e-reader, and conversion can be done
using [ImageMagick convert script](https://www.imagemagick.org/script/convert.php); For example:
```
convert /media/matthias/Vivlio/screens/scr0002.bmp \
        -scale 20% -bordercolor pink -border 5 \
        docs/screenshot-hourly-forecast.jpg
```

## Debugging

### Log file

The application can be started with the `--verbose` command line
argument to generate a detailed log file. The file path is of the form
`/mnt/ext1/system/state/taranis.log` but may vary depending on the use
of profiles.

To specify command line arguments rather use a terminal like
[pbterm](https://github.com/Alastor27/pbterm) or use [Desktop
integration](./docs/desktop_integration.md).

### Remote debugging

One must first start `gdbserver` on the e-reader:

1. Install [pbterm](https://github.com/Alastor27/pbterm)
2. Start `taranis.app`
3. Press the "Home" key to go back to the "desktop"
4. Start `pbterm`
4. Run `TARANIS_PID=$(pgrep taranis.app)` to identify the process running
   `taranis.app`, say 1423
5. Run `gdbserver --attach :10002 ${TARANIS_PID}`

The e-reader must be connected to Internet and its IP address must be
known (eg start `ipconfig` in `pbterm`). It is recommended to disable
automatic poweroff.

On the host computer, start a shell with current working directory the
root directory of a Git clone of taranis repository. Then start GDB:
```shell
./SDK_6.3.0/SDK-B288/usr/bin/arm-linux-gdb -q taranis.app
```

Under GDB, run `target remote 192.168.1.34:10002` where the IP address
must be updated to the address of the e-reader.

## References

* https://github.com/pocketbook-free/InkViewDoc
* https://github.com/JuanJakobo/Pocketbook-Read-offline
* https://github.com/SteffenBauer/PocketPuzzles
* https://github.com/pmartin/pocketbook-demo
* https://github.com/pocketbook-free/sdkrelease_1_1a
