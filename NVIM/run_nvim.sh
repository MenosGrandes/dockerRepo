!#/bin/bash
local MOUNT_PATH=$1
local DOCKER_NAME=$2
docker run -it  -v %MOUNT_PATH%:/code %DOCKER_NAME%
