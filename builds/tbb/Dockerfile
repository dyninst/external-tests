ARG base
FROM ${base}

ARG build_jobs=1

LABEL maintainer="@hainest"

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=America/Chicago

RUN apt-get -qq update && \
    apt -qq install -y --no-install-recommends git ca-certificates

# Remove system tbb so it doesn't interfere
RUN apt -qq remove --purge -y *tbb*

# Remove unneeded Dyninst files
RUN rm -rf /dyninst build.sh

# Build and install all versions of TBB
COPY install.sh test.sh /
RUN bash install.sh $build_jobs
