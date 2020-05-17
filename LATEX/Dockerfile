FROM debian:stable-slim
ENV DEBIAN_FRONTEND=noninteractive
ENV DEBCONF_NONINTERACTIVE_SEEN=true

ENV PACKAGES="\
	git \
	wget \
	make \
	python-pygments \
	texlive-full \
	texlive-latex-extra \
"

RUN apt-get -y -qq update&& apt-get -y -qq install $PACKAGES \
	&& mkdir ~/code \
	&& apt-get -qq autoclean \
	&& apt-get -qq clean \
	&& apt-get -qq autoremove \
	&& rm -rf /var/lib/apt/lists/*
	
ENTRYPOINT ["/bin/bash"]


