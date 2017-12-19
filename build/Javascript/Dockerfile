from 42ua/emsdk

RUN mkdir /plutoboy_js
ADD . /plutoboy_js/
WORKDIR /plutoboy_js/build/Javascript

# Build
CMD make; mkdir -p /mnt/JsOutput; cp gameboy.html gameboy.html.mem gameboy.js /mnt/JsOutput;
