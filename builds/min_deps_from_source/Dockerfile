ARG base
FROM ${base}

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

# Get a copy of Dyninst so we can use the docker/dependencies.versions file
RUN git clone --depth=1 --branch=master https://github.com/dyninst/dyninst

# Build and install all dependencies from source
COPY install.sh .
RUN bash install.sh $build_jobs
