from bkcsoft/pspsdk

# Install SDL and other PSP libraries
RUN git clone https://github.com/pspdev/psplibraries;\ 
    cd psplibraries;\ 
    ./libraries.sh

RUN mkdir /plutoboy_psp
ADD . /plutoboy_psp/
WORKDIR /plutoboy_psp/build/PSP

# Build and copy the EBOOT back to the host
CMD make clean; make; make directory; cp -r Gameboy /mnt;
