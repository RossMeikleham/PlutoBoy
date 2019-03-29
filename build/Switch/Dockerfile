from devkitpro/devkita64:latest

RUN apt-get update && apt-get install -y gcc g++ zip
RUN dkp-pacman -S switch-glad switch-sdl2 switch-sdl2_gfx switch-sdl2_image switch-sdl2_net switch-sdl2_mixer switch-bzip2 --noconfirm


RUN mkdir /plutoboy_Switch
ADD . /plutoboy_Switch/
WORKDIR /plutoboy_Switch/build/Switch

CMD make clean && make && cp Plutoboy.nro /mnt;
