FROM jameyhicks/ubuntu-clang-scons

USER root

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y libsdl1.2-dev libsdl-net1.2-dev\
        libsdl2-dev \
        libsdl2-net-dev

RUN mkdir /plutoboy_linux
COPY ./build /plutoboy_linux/build
COPY ./src/platforms/standard /plutoboy_linux/src/platforms/standard
COPY ./src/shared_libs /plutoboy_linux/src/shared_libs
COPY ./src/core /plutoboy_linux/src/core
COPY ./src/non_core /plutoboy_linux/src/non_core
WORKDIR /plutoboy_linux/build/Unix

ENV cc="gcc" framework="SDL2" mode="release"

CMD cd /plutoboy_linux/build/Unix \
    && scons cc=${cc} framework=${framework} mode=${mode} \
    && cp plutoboy /mnt
