FROM --platform=linux/amd64 debian:stretch

USER root

ARG DEBIAN_FRONTEND=noninteractive

ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8

ENV ANDROID_HOME /opt/android-sdk-linux

RUN sed -i s/deb.debian.org/archive.debian.org/g /etc/apt/sources.list
RUN sed -i 's|security.debian.org|archive.debian.org|g' /etc/apt/sources.list 
RUN sed -i '/stretch-updates/d' /etc/apt/sources.list 

RUN dpkg --add-architecture i386 \
  && apt-get update && apt-get install -y --no-install-recommends expect \
    libncurses5:i386 \
    libstdc++6:i386 \
    zlib1g:i386 \
    build-essential \
    openssh-client \
	curl \
    ant \
	make \
	file \
    unzip \
    wget \ 
    gnupg \
    openjdk-8-jdk \
  && apt-get install -y --no-install-recommends maven \
  && rm -rf /var/lib/apt/lists/*

# Install Android SDK installer
RUN curl -o android-sdk.zip "https://dl.google.com/android/repository/tools_r25.2.5-linux.zip" \
  && unzip -C android-sdk.zip -d "${ANDROID_HOME}" \
  && rm *.zip
  

COPY build/Android/dockerscripts/ /opt/sdk-tools

ENV PATH ${PATH}:${ANDROID_HOME}/tools/bin:${ANDROID_HOME}/tools:${ANDROID_HOME}/platform-tools:/opt/sdk-tools

RUN sdkmanager --list \
  && /opt/sdk-tools/android-accept-licenses.sh "sdkmanager platform-tools" \
  && /opt/sdk-tools/android-accept-licenses.sh "sdkmanager build-tools;26.0.3" \
  && /opt/sdk-tools/android-accept-licenses.sh "sdkmanager sources;android-26" \
  && /opt/sdk-tools/android-accept-licenses.sh "sdkmanager platforms;android-26" \
  && sdkmanager --list


# Install NDK
#RUN apt-get install -y --no-install-recommends p7zip-full
#RUN curl -o ndk.bin http://dl.google.com/android/ndk/android-ndk-r14b-linux-x86_64.bin
#RUN 7za x -o/. ndk.bin 
#RUN chmod a+x ndk.bin
#RUN ./ndk.bin

RUN curl -o ndk.zip https://dl.google.com/android/repository/android-ndk-r14b-linux-x86_64.zip
RUN unzip -q ndk.zip
ENV PATH /android-ndk-r14b:$PATH

# Download SDL
RUN wget https://www.libsdl.org/release/SDL2-2.0.12.tar.gz
RUN tar -xf SDL2-2.0.12.tar.gz 
RUN mv SDL2-2.0.12 /SDL

RUN wget https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.tar.gz
RUN tar -xf SDL2_ttf-2.0.15.tar.gz
RUN mv SDL2_ttf-2.0.15 /SDL_ttf

RUN wget https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.5.tar.gz
RUN tar -xf SDL2_image-2.0.5.tar.gz
RUN mv SDL2_image-2.0.5 /SDL_image

#Setup workdir
#RUN mkdir -p /plutoboy_android/build
#COPY ./build/Android /plutoboy_android/build/Android
#COPY ./src /plutoboy_android/src
#WORKDIR /plutoboy_android/build/Android


CMD cd /mnt/build/Android;\ 
rm /mnt/build/Android/jni/SDL;\
rm /mnt/build/Android/jni/SDL_ttf;\
rm /mnt/build/Android/jni/SDL_image;\
rm /mnt/build/Android/src/main/java/org/libsdl;\
ln -s /SDL  /mnt/build/Android/jni/SDL;\
ln -s /SDL_ttf  /mnt/build/Android/jni/SDL_ttf;\
ln -s /SDL_image  /mnt/build/Android/jni/SDL_image;\
# symlink SDLActivity.java from SDL2
#ln -s /SDL/android-project/src/org/libsdl /mnt/build/Android/src/org/libsdl;\
ln -s /SDL/android-project/app/src/main/java/org/libsdl /mnt/build/Android/src/main/java/org/libsdl;\
ndk-build clean &&\
ndk-build &&\ 
ant clean && \
ant release &&\ 
cd bin &&\
cp SDLActivity-release-unsigned.apk Plutoboy.apk &&\
keytool -genkey\ 
	-noprompt\
	-keystore my-release-key.keystore\ 
	-storepass password123\
 	-keypass password123\
	-alias Plutoboy\ 
	-keyalg RSA -keysize 2048\
	-validity 10000 -dname "CN=RM, OU=RM, O=RM, L=RM, S=RM, C=RM";\
jarsigner -verbose\
	-sigalg SHA1withRSA\
	-digestalg SHA1\
	-keystore my-release-key.keystore\
	-storepass password123\
	-keypass password123\
	Plutoboy.apk Plutoboy &&\
ls &&\
cp Plutoboy.apk /mnt/
