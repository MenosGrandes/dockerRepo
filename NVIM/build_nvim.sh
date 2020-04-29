#!/bin/bash
local CONTAINER_NAME=$1
local DOCKERFILE_PATH=$2
docker build --rm -t $CONTAINER_NAME $DOCKERFILE_PATH
