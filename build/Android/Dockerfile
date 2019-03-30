FROM debian:stretch

USER root

ARG DEBIAN_FRONTEND=noninteractive

ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8

ENV ANDROID_HOME /opt/android-sdk-linux

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
  && /opt/sdk-tools/android-accept-licenses.sh "sdkmanager build-tools;21.1.2" \
  && /opt/sdk-tools/android-accept-licenses.sh "sdkmanager sources;android-21" \
  && /opt/sdk-tools/android-accept-licenses.sh "sdkmanager platforms;android-21" \
  && sdkmanager --list


# Install NDK
RUN curl -o ndk.bin http://dl.google.com/android/ndk/android-ndk-r10e-linux-x86_64.bin
RUN chmod a+x ndk.bin
RUN ./ndk.bin > /dev/null
ENV PATH /android-ndk-r10e:$PATH

# Download SDL
RUN wget https://www.libsdl.org/release/SDL2-2.0.5.tar.gz
RUN tar -xf SDL2-2.0.5.tar.gz 
RUN mv SDL2-2.0.5 /SDL


#Setup workdir
RUN mkdir -p /plutoboy_android/build
COPY ./build/Android /plutoboy_android/build/Android
COPY ./src /plutoboy_android/src
WORKDIR /plutoboy_android/build/Android

RUN ln -s /SDL  /plutoboy_android/build/Android/jni/SDL

# symlink SDLActivity.java from SDL2
RUN ln -s /SDL/android-project/src/org/libsdl  /plutoboy_android/build/Android/src/org/libsdl

CMD ndk-clean;\
ndk-build;\ 
ant clean;\
ant release;\ 
cd bin;\
cp SDLActivity-release-unsigned.apk Plutoboy.apk;\
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
	Plutoboy.apk Plutoboy;\
ls;\
cp Plutoboy.apk /mnt/
