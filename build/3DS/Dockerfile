from devkitpro/devkitarm:latest

RUN mkdir /plutoboy_3DS
ADD . /plutoboy_3DS/
WORKDIR /plutoboy_3DS/build/3DS

CMD make clean && make &&\
    makerom -rsf meta/cia.rsf -elf Plutoboy.elf -icon meta/icon.png -banner meta/banner.png -f cia -o Plutoboy.cia; 
