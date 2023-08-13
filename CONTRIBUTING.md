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

1. First, clone the source repository and populate Git submodules:
   ```sh
   git submodule init
   git submodule update
   ```
   
   This will populate the `SDK_6.3.0` directory with the PocketBook
   SDK.

2. Generate the cross compilation `crossfile_arm.ini` file using the provided template
   [crossfile_arm.ini.in](./crossfile_arm.ini.in):
   ```sh
   export PWDESC=$(echo $PWD | sed 's_/_\\/_g')
   sed "s/@pwd@/$PWDESC/g" crossfile_arm.ini.in > crossfile_arm.ini
   ```

3. Download, build and install source code of the [GNU Scientific
   Library dependency](https://www.gnu.org/software/gsl/):
   ```sh
   mkdir 3rd-parties
   pushd 3rd-parties
   wget https://ftp.gnu.org/gnu/gsl/gsl-2.7.1.tar.gz
   tar -xzf gsl-2.7.1.tar.gz
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
$ convert /media/matthias/Vivlio/screens/scr0002.bmp \
          -scale 40% -bordercolor pink -border 5 \
		  docs/screenshot-hourly-forecast.jpg
```

## References

* https://github.com/pocketbook-free/InkViewDoc
* https://github.com/JuanJakobo/Pocketbook-Read-offline
* https://github.com/SteffenBauer/PocketPuzzles
* https://github.com/pmartin/pocketbook-demo
