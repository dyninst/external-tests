ARG base
FROM ${base}

ARG build_jobs=1

LABEL maintainer="@hainest"

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=America/Chicago

RUN apt -qq install -y --no-install-recommends curl wget

# Remove system Boost so it doesn't interfere
RUN apt -qq remove --purge -y libboost*

# Remove unneeded Dyninst files
RUN rm -rf /dyninst build.sh

# Build and install all versions of Boost
COPY install.sh .
RUN bash install.sh ${build_jobs}