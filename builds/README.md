## Builds

Build Dyninst against multiple versions of each of its dependencies.

### Files

Each build type has three main files:

#### Dockerfile

This sets up the OS environment based on the ghcr.io/dyninst/amd64/<OS>-<VERSION>-base container. There isn't a need to have multiple OS's at this time, so only Ubuntu 20.04 is used. Importantly, the library under test must be removed from the base build so that it does not interfere with the tests.

#### install.sh

The script to build all relevant versions of the library using the relevant release tags from its source repository where the minimum accepted version is taken from Dyninst's docker/dependencies.versions file. When building the image from scratch, this step may fail when there are many versions to download due to bandwidth throttling.

By convention, each version of the dependency is stored in the root directory of the container image. Some dependencies are large enough that they need to be compressed to reduce the image size (see the Boost image, for example). In these cases, the archives must be expanded in ``test.sh`` before they can be used.
 
#### test.sh
 
The script to build Dyninst against all of the versions of the dependency with ``DYNINST_WARNINGS_AS_ERRORS=ON``.