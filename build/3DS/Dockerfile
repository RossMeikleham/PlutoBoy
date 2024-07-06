from devkitpro/devkitarm:20240511

RUN apt-get update && apt-get install -y gcc g++ zip cmake

# Install makerom
RUN cd / && git clone https://github.com/profi200/Project_CTR
RUN cd /Project_CTR/makerom && git checkout 8a9f9bda55d71274799eacf && make
ENV PATH="/Project_CTR/makerom/:${PATH}"

# Install bannertool
RUN cd / && git clone https://github.com/carstene1ns/3ds-bannertool
RUN cd /3ds-bannertool && cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo && cmake --build build
ENV PATH="/3ds-bannertool/build/:${PATH}"

RUN mkdir /plutoboy_3DS
ADD . /plutoboy_3DS/
WORKDIR /plutoboy_3DS/build/3DS

CMD make clean &&\
    make && \
    bannertool makebanner -i banner.png -a jingle.wav -o banner.bnr &&\
    makerom -rsf Plutoboy.rsf -elf Plutoboy.elf -icon Plutoboy.smdh -banner banner.bnr -f cia -o Plutoboy.cia; cp Plutoboy.3dsx Plutoboy.cia /mnt; 
