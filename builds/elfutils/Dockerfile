ARG base
FROM ${base}

ARG build_jobs=1

LABEL maintainer="@hainest"

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=America/Chicago

RUN apt-get -qq update && \
    apt -qq install -y --no-install-recommends git ca-certificates

# Remove system elfutils so it doesn't interfere
RUN apt -qq remove --purge -y elfutils* libelf* libdw* libdebug*
RUN rm -rf /usr/local/include/{dwarf.h,gelf.h,libelf.h,elfutils} /usr/local/lib/lib{elf,dw,debuginfod}*.so

# Remove unneeded Dyninst files
RUN rm -rf /dyninst build.sh

# Build and install all versions of elfutils
COPY build.sh .
RUN bash build.sh $build_jobs
