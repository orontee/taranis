# How-to release

Sticky note for maintainers.

## Identify the new version "number"

The project uses [Semantic Versioning
Specification](https://semver.org/), thus the new version number
should be easy to choose from the content of the [NEWS.md](../NEWS.md)
file.

Remember that pre-releases version must end with the `-rc[0-9]+`
regular expression.

## Commit new version number, tag and push

Once the version is known, say `vX.Y.Z` (to simplify), one can:

1. On a feature branch or a release branch, commit "Prepare release of
   version X.Y.Z" with:
   - Update of version number in [CMakeLists.txt](../CMakeLists.txt)
   - Rename the "Unreleased" section in [NEWS.md](../NEWS.md).

3. Tag previous commit with `vX.Y.Z`.

4. Push both commit and tag to GitHub.

## Github release workflow

A Github action workflow is automatically created when new tags are
pushed. It creates a Github draft release (or pre-release depending on
the Git tag format).

## Post-build checks

Once the workflow is finished, download the generated artefact,
install and test.

1. If everything is ok, edit release to uncheck the "Set as draft" box
   and, in case of a release, check the "Set as the latest release"
   box.

2. For a release, post a message on [MobileRead forum dedicated to
    PocketBook](https://www.mobileread.com/forums/forumdisplay.php?f=206)
    🎆
