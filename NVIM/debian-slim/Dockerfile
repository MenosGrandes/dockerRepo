FROM debian:stable-slim

ENV PACKAGES="\
	git \
	curl \
	python3-dev \
	python3-pip \
	wget \
"

ENV PYTHON_PACKAGES="\
	numpy \
	matplotlib \
	neovim \
	scipy\
	pandas\
	sympy\
	nose\
	flake8 \
	autopep8 \
	jedi \
	scipy \
"

RUN apt-get -y -qq update&& apt-get -y -qq install $PACKAGES \
	&& curl -sL https://deb.nodesource.com/setup_13.x | bash - && apt-get install -y -qq nodejs && npm install -g neovim \
	&& python3 -m pip install --user $PYTHON_PACKAGES \
	&& wget --no-check-certificate --content-disposition https://github.com/neovim/neovim/releases/download/v0.4.3/nvim.appimage \
	&& chmod u+x nvim.appimage \
	&& ./nvim.appimage --appimage-extract \
	&& echo 'alias nvim="/squashfs-root/usr/bin/nvim"' >> ~/.bashrc \
	&& mkdir -p ~/.config \
	&& mkdir -p ~/.config/nvim \
	&& git clone https://github.com/MenosGrandes/dots.git ~/dots \
	&& mv  ~/dots/vim/init.vim ~/.config/nvim/ \
	&& mv  ~/dots/vim/coc-settings.json ~/.config/nvim/ \
	&& ./squashfs-root/usr/bin/nvim +PlugInstall +qall \
	&& mkdir -p /root/.config/coc/extensions \
	&& cd /root/.config/coc/extensions \
	&& echo '{"dependencies":{}}'> package.json \
	&& npm install coc-python --global-style --ignore-scripts --no-bin-links --no-package-lock --only=prod \
	&& /squashfs-root/usr/bin/nvim +CocUpdateSync +qall \
	&& mkdir /code \
        && rm -rf ~/dots



#CMD ["./squashfs-root/usr/bin/nvim"]

