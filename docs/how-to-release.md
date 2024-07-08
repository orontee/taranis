# How-to release

Sticky note for maintainers.

1. Identify new version "number", say `1.9.0`, according to [Semantic
   Versioning Specification](https://semver.org/) and
   [NEWS.md](../NEWS.md) content.

2. On `main` branch, commit "Prepare release of version 1.9.0" with:
   - Update of version number in [meson.build](../meson.build)
   - Rename the "Unreleased" section in [NEWS.md](../NEWS.md).

3. Tag previous commit with `v1.9.0` (or `v1.9.0-rc0`, `v1.9.0-rc1`,
   etc. if release candidates are needed).

4. Push previous commit and tag to GitHub.

5. From GitHub website, run the `build-application` action for the
   `main` branch and the new tag.

6. Download generated artefact and unzip the downloaded file.

7. [Draft a new
   release](https://github.com/orontee/taranis/releases/new) named
   "Release 1.9.0" with content:

        See [News file](https://github.com/orontee/taranis/blob/v1.9.0/NEWS.md) for release content.

        **Full Changelog**: https://github.com/orontee/taranis/compare/v1.8.0...v1.9.0

        ## How to install

        Install by copying the `taranis.pbi` file to the e-book reader, then open the corresponding "book" using the library application.

        An alternative is to manually copy the files from the `taranis.zip` archive to the `applications` directory of the e-book reader.

   ⚠️ Check the "Set as pre-release" box

   Attach the following files:
   - `taranis.pbi`
   - `taranis.sha256`
   - `taranis.zip`

   Then save the release draft.

8. Download installer from release; Install and test.

9. If everything is ok, edit release to uncheck the "Set as
   pre-release" box and check the "Set as the latest release" box.

10. Advertise new release on [MobileRead forum dedicated to
    PocketBook](https://www.mobileread.com/forums/forumdisplay.php?f=206) 🎆
