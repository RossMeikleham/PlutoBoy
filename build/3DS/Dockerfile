from devkitpro/devkitarm:latest

RUN mkdir /plutoboy_3DS
ADD . /plutoboy_3DS/
WORKDIR /plutoboy_3DS/build/3DS

CMD make clean && make && 3dslink -a 192.168.0.20 Plutoboy.3dsx; 
