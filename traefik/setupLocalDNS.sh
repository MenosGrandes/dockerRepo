#/bin/bash
docker ps --format "192.168.0.102 {{.Names}}.docker" >> /etc/hosts
