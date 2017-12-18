from miladiir/vitasdk:latest

ENV VITASDK /usr/local/vitasdk

# Install SDL2 and other VitaSDK libraries
RUN git clone https://github.com/vitasdk/vdpm;\ 
    cd vdpm;\ 
    ./install-all.sh;

RUN mkdir /plutoboy_vita
ADD . /plutoboy_vita/
WORKDIR /plutoboy_vita/build/Vita

RUN mkdir build

# Build and copy the VPK back to the host
CMD cd build; cmake ..; make; cp Plutoboy.vpk /mnt;
