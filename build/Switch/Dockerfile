FROM devkitpro/devkita64:latest

#RUN apt-get update && apt-get install -y gcc g++ zip
#RUN dkp-pacman -Syu switch-dev switch-glad switch-sdl2 switch-sdl2_gfx switch-sdl2_image switch-sdl2_net switch-sdl2_mixer switch-bzip2 --noconfirm

RUN apt-get update && \
    apt-get install -y unzip && \
    dkp-pacman -Syu --noconfirm && \
    dkp-pacman -S --noconfirm devkitARM switch-mesa switch-glad devkitA64 libnx switch-tools



RUN mkdir /plutoboy_Switch
ADD . /plutoboy_Switch/
WORKDIR /plutoboy_Switch/build/Switch

CMD make clean && make && cp Plutoboy.nro /mnt;
