xhost +si:localuser:root 
sudo docker run --runtime=nvidia -ti --rm -e DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix pupil_labs:2.0 /bin/bash
