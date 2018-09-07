from devkitpro/devkitarm:latest

RUN apt-get update && apt-get install -y gcc g++ zip

# Install makerom
RUN cd / && git clone https://github.com/profi200/Project_CTR
RUN cd /Project_CTR/makerom && make
ENV PATH="/Project_CTR/makerom/:${PATH}"

# Install bannertool
RUN cd / && git clone https://github.com/Steveice10/bannertool
RUN cd /bannertool && git submodule update --init --recursive && make 
ENV PATH="/bannertool/output/linux-x86_64/:${PATH}"

RUN mkdir /plutoboy_3DS
ADD . /plutoboy_3DS/
WORKDIR /plutoboy_3DS/build/3DS

CMD make clean && make && \
    bannertool makebanner -i banner.png -a jingle.wav -o banner.bnr &&\
    makerom -rsf Plutoboy.rsf -elf Plutoboy.elf -icon Plutoboy.smdh -banner banner.bnr -f cia -o Plutoboy.cia; cp Plutoboy.3dsx Plutoboy.cia /mnt; 
