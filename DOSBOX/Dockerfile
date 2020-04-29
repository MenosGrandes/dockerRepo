FROM debian:stable-slim
ENV USER=root
ENV PASSWORD=password1

COPY AK /dos/AK
#silent apt
RUN echo 'Dpkg::Use-Pty "0";' > /etc/apt/apt.conf.d/00usepty
ENV DEBIAN_FRONTEND noninteractive
ENV DEBCONF_NONINTERACTIVE_SEEN true

RUN  apt-get -qq  update  < /dev/null > /dev/null \
 &&  apt-get -qq  install -y --no-install-recommends tightvncserver ratpoison dosbox novnc websockify < /dev/null > /dev/null \
 && mkdir ~/.vnc/ \
 && mkdir ~/.dosbox \
 && echo $PASSWORD | vncpasswd -f > ~/.vnc/passwd \
 && chmod 0600 ~/.vnc/passwd \
 && echo "set border 0" > ~/.ratpoisonrc  \
 && echo "exec dosbox -conf ~/.dosbox/dosbox.conf -fullscreen -conf ~/.dosbox/dosbox.conf ">> ~/.ratpoisonrc \
 && export DOSCONF=$(dosbox -printconf) \
 && cp $DOSCONF ~/.dosbox/dosbox.conf \
 && sed -i 's/usescancodes=true/usescancodes=false/' ~/.dosbox/dosbox.conf \
 && sed -i 's/autolock=true/autolock=false/' ~/.dosbox/dosbox.conf \
 && echo  "MOUNT C: /dos">> ~/.dosbox/dosbox.conf \
 && bash -c 'echo -e 'path=c:/\AK/\borlandc/\bin' >> ~/.dosbox/dosbox.conf' \
 && echo  "c:">> ~/.dosbox/dosbox.conf \
 && openssl req -x509 -nodes -newkey rsa:2048 -keyout ~/novnc.pem -out ~/novnc.pem -days 3650 -subj "/C=US/ST=NY/L=NY/O=NY/OU=NY/CN=NY emailAddress=email@example.com" \
 && apt-get -qq autoclean \
 && apt-get -qq clean \
 && apt-get -qq autoremove \
 && rm -rf /var/lib/apt/lists/*
EXPOSE 6080
CMD vncserver && websockify -D --web=/usr/share/novnc/ --cert=~/novnc.pem 6080 localhost:5901 && tail -f /dev/null