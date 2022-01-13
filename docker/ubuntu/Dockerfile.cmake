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
    libssl-dev
    

RUN git clone --depth 1 --branch master https://github.com/dyninst/testsuite testsuite/src

RUN pip3 install clingo && \
    git clone --depth 1 --branch develop https://github.com/spack/spack && \
    ln -s /spack/bin/spack /usr/bin/spack && \
    spack compiler find && \
    spack external find --not-buildable gcc && \
    ssl_version=$(apt show libssl-dev | perl -ne 'print $1 if m/version\:\s*(\d+\.\d+\.\d+\w)/i') && \
    ncurses_version=$(apt show libncurses-dev | perl -ne 'print $1 if m/version\:\s*(\d+\.\d+)/i') && \
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
" >> ~/.spack/packages.yaml

COPY builds/cmake/* /

# Remove system cmake so it doesn't interfere
RUN apt -qq remove --purge -y cmake*
