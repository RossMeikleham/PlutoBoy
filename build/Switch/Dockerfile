FROM devkitpro/devkita64

RUN mkdir /plutoboy_Switch
ADD . /plutoboy_Switch/
WORKDIR /plutoboy_Switch/build/Switch

CMD make clean && make && cp Plutoboy.nro /mnt;
