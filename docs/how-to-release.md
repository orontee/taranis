# How-to release

Sticky note for maintainers.

1. Identify new version "number", say `1.9.0`, according to [Semantic
   Versioning Specification](https://semver.org/) and
   [NEWS.md](../NEWS.md) content.

2. On `main` branch, commit "Prepare release of version 1.9.0" with:
   - Update of version number in [CMakeLists.txt](../CMakeLists.txt)
   - Rename the "Unreleased" section in [NEWS.md](../NEWS.md).

3. Tag previous commit with `v1.9.0` (or `v1.9.0-rc0`, `v1.9.0-rc1`,
   etc. if release candidates are needed).

4. Push both commit and tag to GitHub.

5. A Github action workflow is automatically created when new tags are
   pushed. It creates a Github draft release (or pre-release depending
   on the Git tag format).
   
6. Once the workflow is finished, download the generated artefact,
   install and test.

7. If everything is ok, edit release to uncheck the "Set as draft" box
   and, in case of a release, check the "Set as the latest release"
   box.

10. For a release, post a message on [MobileRead forum dedicated to
    PocketBook](https://www.mobileread.com/forums/forumdisplay.php?f=206)
    🎆
