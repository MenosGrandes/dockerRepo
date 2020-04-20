FROM debian:stable-slim

RUN apt-get -y -qq update&& apt-get -y -qq install git \
			   curl \
			   python3-pip \
			   wget


RUN curl -sL https://deb.nodesource.com/setup_13.x | bash -
RUN apt-get install -y -qq nodejs
RUN npm install -g neovim
RUN python3 -m pip install --user pynvim

RUN wget --no-check-certificate --content-disposition https://github.com/neovim/neovim/releases/download/v0.4.3/nvim.appimage && chmod u+x nvim.appimage && ./nvim.appimage --appimage-extract 

RUN echo 'alias nvim="./squashfs-root/usr/bin/nvim"' >> ~/.bashrc

RUN mkdir -p ~/.config && mkdir -p ~/.config/nvim && git clone https://github.com/MenosGrandes/dots.git ~/dots && mv  ~/dots/vim/init.vim ~/.config/nvim/ && ./squashfs-root/usr/bin/nvim +PlugInstall +qall





