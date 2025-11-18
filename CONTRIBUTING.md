# Contributing to Taranis

## Dependencies

### PocketBook SDK

The [PocketBook SDK](https://github.com/pocketbook/SDK_6.8.0) is
hosted on Github and used to be managed as a Git submodule. Recent
versions are distributed as archive in the Github project release
artefacts.

### GNU Scientific Library

The [GNU Scientific Library
dependency](https://www.gnu.org/software/gsl/) is used to interpolate
hourly temperatures and draw a smooth curve. It must be cross-compiled
and installed manually, see instructions below.

## Build

### Using a container

The SDK has hidden dependencies (`libtinfo5`); They are quite old and
it may be difficult to install them on a modern Linux host. To get
around this problem, a container image is provided.

To build in a container:
```sh
$ buildah build --build-arg DEVICE_FAMILY=B288 --tag pbsdk:6.8-B288 .
$ podman run --rm -ti \
             --volume ${PWD}:/src \
			 --hostname pbsdk-B288 \
			 --name pbsdk-B288 \
			 localhost/pbsdk:6.8-B288 \
             bash
roo@pb-build-env:/src$ source /opt/SDK-6.8/build_env.sh
roo@pb-build-env:/src$ cmake -S . -B build \
                             -DCMAKE_TOOLCHAIN_FILE=${SDK_ROOT_PATH}/share/cmake/arm_conf.cmake
roo@pb-build-env:/src$ cmake --build build
```

Custom targets `installer`, `archive` and `checksum` are provided to
automate the application packaging.

### Build through GitHub action

The GitHub action `build-application` allows to simultaneously build
the application, its installer, and an archive.

It's mainly used to release the application, see [How-to
release](./docs/how-to-release.md).

## Translations

See [the dedicated file](./po/README.md) to read about contributing to
translations.

This section is dedicated to technical considerations.

### Updating PO files

To keep things simple, Taranis uses the custom translation mechanism
provided by Inkview SDK, but:

* Translatable strings are gathered in the [taranis.pot
  file](./taranis.pot).

* Translations are stored in [PO files](./) which are textual,
  editable files.

* The extraction of translatable strings and updates of PO files is
  done by the common `xgettext` and `msgmerge` utilities.
  
  Thus to update the pot file and translation files to match current
  source code, one must run:
  
  ``` bash
  xgettext --package-name=taranis \
           --foreign-user \
           --from-code=UTF-8 \
           --add-comments \
           --keyword=GetLangText \
           --keyword=GetLangTextF \
           --keyword=GetLangTextPlural \
           --output-dir=po \
           --output=taranis.pot \
           src/*.cc src/*.h
  ```
  
  And:
  
  ``` bash
  cat po/LINGUAS | xargs --replace msgmerge --update po/{}.po po/taranis.pot
  ```
  
* The build target has been extended to automatically generate a C++
  file (matching the expectations of Inkview SDK) from all PO files, see
  [generate_l10n_cc.py](../scripts/generate_l10n_cc.py).

### Integration of new language

* Weblate commits should extend [po/LINGUAS](./po/LINGUAS) and add a
  `.po` file in the [po](./po) directory.
* Support for the new language must be explicitly added to
  [src/l10n.h](./src/l10n.h)
* Then rebuild the application

## Tooling

Format sources:
```sh
clang-format -i src/*.h src/*.cc
```

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

### SSH connection with device

Use the USB connection (it's the last time you'll use it 🎆) to create
a file `/mnt/ext1/applications/SSHServer.app` on the device with
following content where one must adapt the IP address to match the SSH
client address (a [more complete version](./scripts/SSHServer.app) is
available from the `scripts` directory):

```sh
#!/bin/sh

TRUSTED_CLIENT_IP=192.168.1.22
PORT=8222

dropbear -p ${PORT} -G ${TRUSTED_CLIENT_IP}
```

Make sure Wifi is enabled on the device, identify its IP address
(using PBTerm and `ipconfig` for example, let's say that in our case
it's 192.168.1.34), and start the "SSHServer" application just created.

⚠️ Note that from now on (until a reboot or an explicit kill of the
dropbear process) the device will be running a SSH server accepting
unauthenticated connections on port 8222 from the 192.168.1.22 IP
address.

Then, on the client, create or complete the file `~/.ssh/config`:

```
Host 192.168.1.34
     User reader
     PubkeyAcceptedAlgorithms +ssh-rsa
     HostkeyAlgorithms +ssh-rsa
     Port 8222
```

Finally test the connection from the client with `ssh 192.168.1.34
ash`. Beware that there's no allocated pseudo-terminal…

Since there's no SFTP server on the device, to copy files from the
client to the device one must redirect standard input to the target
file. For example, to copy taranis executable after a build:

```sh
cat builddir/artifact/taranis.app | \
  ssh 192.168.1.34 '( cat - > /mnt/ext1/applications/taranis.app )'
```

(make sure the file isn't opened on the device side).

To track taranis logs:

```sh
ssh 192.168.1.34 tail -f /mnt/ext1/system/profiles/Matthias/state/taranis.log
```

### Remote debugging

⚠️ First, make sure `taranis.app` has been build with debug symbols.

One must start `gdbserver` on the e-reader:

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
root directory of a Git clone of `taranis` repository. Then start GDB:
```shell
./SDK_6.8.0/SDK-B288/usr/bin/arm-linux-gdb -q taranis.app
```

Under GDB, run `target remote 192.168.1.34:10002` where the IP address
must be updated to the address of the e-reader.

## References

* https://github.com/pocketbook-free/InkViewDoc
* https://github.com/JuanJakobo/Pocketbook-Read-offline
* https://github.com/SteffenBauer/PocketPuzzles
* https://github.com/pmartin/pocketbook-demo
* https://github.com/pocketbook-free/sdkrelease_1_1a
