FROM ubuntu:20.04

ARG build_jobs=1

LABEL maintainer="@hainest"

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=America/Chicago

RUN apt-get -qq update && \
    apt -qq install -y --no-install-recommends \
    build-essential \
    curl            \
    wget            \
    git             \
    cmake           \
    gcc             \
    g++             \
    ca-certificates \
    gettext         \
    bzip2           \
    zlib1g-dev      \
    m4              \
    libiberty-dev

# Build and install all dependencies from source
COPY test.sh /
