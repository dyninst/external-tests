ARG base
FROM ${base}

LABEL maintainer="@hainest"

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=America/Chicago

RUN apt-get -qq update && \
    apt -qq install -y --no-install-recommends git ca-certificates valgrind gcc-multilib

COPY build.pl test.sh /
