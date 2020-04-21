FROM alpine:latest

RUN apk add --no-cache --quiet --update-cache nodejs \
	npm \
	curl \
	git \
	python3-dev \
	gcc \
	g++ \
	wget \
	neovim


RUN npm install -g neovim
RUN python3 -m pip install --user pynvim


RUN   mkdir -p ~/.config && mkdir -p ~/.config/nvim && git clone https://github.com/MenosGrandes/dots.git ~/dots && mv  ~/dots/vim/init.vim ~/.config/nvim/ 
RUN nvim +PlugInstall +qall && mkdir /code

CMD ["nvim"]  

