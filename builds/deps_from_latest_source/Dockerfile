FROM ubuntu:20.04

LABEL maintainer="@hainest"

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=America/Chicago

RUN apt-get -qq update && \
    apt -qq install -y --no-install-recommends \
    build-essential \
    git             \
    cmake           \
    gcc             \
    g++             \
    ca-certificates \
    gettext         \
    bzip2           \
    zlib1g-dev      \
    m4              \
    libiberty-dev   \
    autoconf        \
    autopoint       \
    pkgconf         \
    flex            \
    bison           \
    gawk            \
    automake        \
    libtool

# Build and install all dependencies from source
COPY install.sh test.sh /
RUN bash install.sh
