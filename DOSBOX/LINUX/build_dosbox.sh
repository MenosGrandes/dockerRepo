#!/bin/bash
CONTAINER_NAME=$1
DOCKERFILE_PATH=$2
DOCKER_BUILDKIT=1 docker build --progress=plain -t $CONTAINER_NAME $DOCKERFILE_PATH
