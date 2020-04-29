FROM ubuntu:bionic
ENV USER=root
ENV PASSWORD=password1
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
#RUN echo 'SUBSYSTEM=="usb",  ENV{DEVTYPE}=="usb_device", GROUP="plugdev", MODE="0664"' > /etc/udev/rules.d/10-libuvc.rules

#RUN udevadm trigger


# Upgrade pip to latest version. This is necessary for some dependencies.
RUN python3 -m pip install --upgrade pip

RUN python3 -m pip install --user cysignals --upgrade
RUN python3 -m pip install --user cython --upgrade
RUN python3 -m pip install --user numexpr --upgrade
RUN python3 -m pip install --user packaging --upgrade
RUN python3 -m pip install --user psutil --upgrade
RUN python3 -m pip install --user pyaudio --upgrade
RUN python3 -m pip install --user pyopengl --upgrade
RUN python3 -m pip install --user pyzmq --upgrade
RUN python3 -m pip install --user scipy --upgrade
RUN python3 -m pip install --user git+https://github.com/zeromq/pyre --upgrade
RUN python3 -m pip install --user pupil-apriltags --upgrade
RUN python3 -m pip install --user pupil-detectors --upgrade
RUN python3 -m pip install --user git+https://github.com/pupil-labs/PyAV --upgrade
RUN python3 -m pip install --user git+https://github.com/pupil-labs/pyuvc --upgrade
RUN python3 -m pip install --user git+https://github.com/pupil-labs/pyndsi --upgrade
RUN python3 -m pip install --user git+https://github.com/pupil-labs/pyglui --upgrade
RUN python3 -m pip install --user msgpack==0.5.6


RUN ldconfig

RUN  apt-get -qq  update  < /dev/null > /dev/null
RUN  apt-get -qq  install -y --no-install-recommends tightvncserver  dbus-x11 xfonts-base xfce4 supervisor xfce4-session xfce4-terminal xterm novnc websockify < /dev/null > /dev/null
RUN mkdir ~/.vnc/
RUN echo $PASSWORD | vncpasswd -f > ~/.vnc/passwd
RUN chmod 0600 ~/.vnc/passwd
RUN openssl req -x509 -nodes -newkey rsa:2048 -keyout ~/novnc.pem -out ~/novnc.pem -days 3650 -subj "/C=US/ST=NY/L=NY/O=NY/OU=NY/CN=NY emailAddress=email@example.com"
EXPOSE 6081
CMD vncserver && websockify -D --web=/usr/share/novnc/ --cert=~/novnc.pem 6081 localhost:5901 && tail -f /dev/null && startxfce4
