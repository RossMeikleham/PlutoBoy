FROM rojuinex/edk2-uefi

RUN mkdir -p /opt/src/edk2/plutoboy/build
RUN mkdir -p /opt/src/edk2/plutoboy/src/platforms/UEFI
RUN mkdir -p /opt/src/edk2/plutoboy/src/shared_libs/UEFI
RUN mkdir -p /opt/src/edk2/plutoboy/src/core
RUN mkdir -p /opt/src/edk2/plutoboy/src/non_core

COPY ./build/UEFI/ /opt/src/edk2/plutoboy/build
COPY ./src/platforms/UEFI /opt/src/edk2/plutoboy/src/platforms/UEFI
COPY ./src/shared_libs/ /opt/src/edk2/plutoboy/src/shared_libs/
COPY ./src/core /opt/src/edk2/plutoboy/src/core
COPY ./src/non_core /opt/src/edk2/plutoboy/src/non_core


WORKDIR /opt/src/edk2/

RUN make -C BaseTools

ENTRYPOINT []

CMD /bin/bash -c ". edksetup.sh && \
    export EDK_TOOLS_PATH=/opt/src/edk2/BaseTools && \
    . edksetup.sh BaseTools && \
    EDK2=1 build -p plutoboy/build/Plutoboy.dsc -a X64 -t GCC5 && \
    mkdir -p /mnt/bin && \
    cp Build/Plutoboy/DEBUG_GCC5/X64/Plutoboy.efi /mnt/bin"
