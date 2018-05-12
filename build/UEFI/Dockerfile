FROM rojuinex/edk2-uefi

RUN mkdir -p /opt/src/edk2/plutoboy/build
RUN mkdir -p /opt/src/edk2/plutoboy/src/platforms/UEFI

COPY ./build/UEFI/ /opt/src/edk2/plutoboy/build
COPY ./src/platforms/UEFI /opt/src/edk2/plutoboy/src/platforms/UEFI

WORKDIR /opt/src/edk2/

RUN make -C BaseTools

ENTRYPOINT []

CMD /bin/bash -c ". edksetup.sh && \
    export EDK_TOOLS_PATH=/opt/src/edk2/BaseTools && \
    . edksetup.sh BaseTools && \
    build -p plutoboy/build/Plutoboy.dsc -a X64 -t GCC5 && \
    mkdir -p /mnt/bin && \
    cp Build/Plutoboy/DEBUG_GCC5/X64/Plutoboy.efi /mnt/bin"
