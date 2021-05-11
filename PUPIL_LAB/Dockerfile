FROM nvidia/cudagl:11.2.2-devel-ubuntu18.04
ENV DEBIAN_FRONTEND noninteractive
ENV DEBCONF_NONINTERACTIVE_SEEN true
RUN apt-get update


RUN apt-get install -yq --no-install-recommends \
pkg-config zip fakeroot git cmake build-essential nasm wget curl \
libusb-1.0-0-dev libglew-dev libglfw3-dev libtbb-dev \
libavformat-dev libavcodec-dev libavdevice-dev libavutil-dev libswscale-dev \
libavresample-dev ffmpeg x264 x265 libportaudio2 portaudio19-dev \
libboost-dev libboost-python-dev libgoogle-glog-dev libatlas-base-dev libeigen3-dev libsuitesparse-dev libopencv-dev \
libceres1 libceres-dev autoconf automake libtool

RUN  apt-get install -y pkg-config git cmake build-essential nasm wget python3-setuptools libusb-1.0-0-dev  python3-dev python3-pip python3-numpy python3-scipy libglew-dev libglfw3-dev libtbb-dev


# ffmpeg >= 3.2
RUN apt-get install -y libavformat-dev libavcodec-dev libavdevice-dev libavutil-dev libswscale-dev libavresample-dev ffmpeg x264 x265 libportaudio2 portaudio19-dev

# OpenCV >= 3
RUN apt-get install -y python3-opencv libopencv-dev

# 3D Eye model dependencies
RUN apt-get install -y libgoogle-glog-dev libatlas-base-dev libeigen3-dev
RUN apt-get install -y libceres-dev


RUN wget -O libjpeg-turbo.tar.gz https://sourceforge.net/projects/libjpeg-turbo/files/1.5.1/libjpeg-turbo-1.5.1.tar.gz/download \
&& tar xvzf libjpeg-turbo.tar.gz \
&& cd libjpeg-turbo-1.5.1 \
&& ./configure --enable-static=no --prefix=/usr/local \
&& make install \
&& ldconfig

RUN git clone https://github.com/pupil-labs/libuvc \
&& cd libuvc\
&& mkdir build\
&& cd build\
&& cmake .. \
&& make \
&& make install

RUN mkdir -p /root/code
RUN echo "Don't forget to download the code and install deps"
COPY  installInDocker.bash /root
