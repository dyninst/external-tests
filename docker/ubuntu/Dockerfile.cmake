ARG base
FROM ${base}

LABEL maintainer="@hainest"

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=America/Chicago

RUN apt -qq install -y --no-install-recommends \
    python3         \
    python3-pip     \
    git             \
    curl            \
    libncurses-dev  \
    libssl-dev		\
    libexpat1-dev	\
    libarchive-dev
    

RUN git clone --depth 1 --branch master https://github.com/dyninst/testsuite testsuite/src

RUN pip3 install clingo && \
    git clone --depth 1 --branch develop https://github.com/spack/spack && \
    ln -s /spack/bin/spack /usr/bin/spack && \
    spack compiler find && \
    spack external find --not-buildable gcc curl && \
    ssl_version=$(apt show libssl-dev | perl -ne 'print $1 if m/version\:\s*(\d+\.\d+\.\d+\w)/i') && \
    ncurses_version=$(apt show libncurses-dev | perl -ne 'print $1 if m/version\:\s*(\d+\.\d+)/i') && \
    expat_version=$(apt show libexpat1-dev | perl -ne 'print $1 if m/version\:\s*(\d+\.\d+\.\d+)/i') && \
    archive_version=$(apt show libarchive-dev | perl -ne 'print $1 if m/version\:\s*(\d+\.\d+\.\d+)/i') && \
printf "\
  openssl:\n\
    externals:\n\
    - spec: openssl@${ssl_version}\n\
      prefix: /usr\n\
    buildable: false\n\
  ncurses:\n\
    externals:\n\
    - spec: ncurses@${ncurses_version}\n\
      prefix: /usr\n\
    buildable: false\n\
  expat:\n\
    externals:\n\
    - spec: expat@${expat_version}\n\
      prefix: /usr\n\
    buildable: false\n\
  libarchive:\n\
    externals:\n\
    - spec: libarchive@${archive_version}\n\
      prefix: /usr\n\
    buildable: false\n\
" >> ~/.spack/packages.yaml

COPY builds/cmake/* builds/run.sh /

# Remove system cmake so it doesn't interfere
RUN apt -qq remove --purge -y cmake*
