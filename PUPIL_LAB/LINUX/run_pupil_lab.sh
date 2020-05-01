#!/bin/bash
MOUNT_PATH=$1
docker run -d -p 6081:6081 -v $MOUNT_PATH:/code pupil_lab
xdg-open  http://localhost:6081/vnc.html
