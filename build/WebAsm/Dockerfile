from 42ua/emsdk

RUN mkdir /plutoboy_js
ADD . /plutoboy_js/
WORKDIR /plutoboy_js/build/WebAsm

# Build
CMD make;\ 
    mkdir -p /mnt/WebAsmFiles;\ 
    cp plutoboy.html plutoboy.wasm plutoboy.js /mnt/WebAsmFiles;\
    mv /mnt/WebAsmFiles/plutoboy.html /mnt/WebAsmFiles/index.html
