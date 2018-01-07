from nold360/kallistios-sdk:latest

USER root

RUN mkdir /plutoboy_dc
ADD . /plutoboy_dc/
WORKDIR /plutoboy_dc/build/Dreamcast

# Build and copy the ISO back to the host
CMD ./DockerBuildIso.sh
