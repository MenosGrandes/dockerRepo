#!/bin/bash
CONTAINER_NAME=$1
IMAGE_NAME=$2
VBOX_MACHINE_IP=$3
#  VBOX_MACHINE_IP must be an IP of machine that VBoxServer is running
#to proper run the VBoxServer the IP must be set explicitly:
# .\VBoxWebSrv.exe --host 192.168.0.165 
#Additionaly to the VBOX_MACHINE_IP must contain the port on which VBoxServer is running. Default port is 18083
docker run --name $CONTAINER_NAME --restart=always -p 85:80 -e ID_HOSTPORT=$VBOX_MACHINE_IP  -e ID_NAME=localhost -d $IMAGE_NAME