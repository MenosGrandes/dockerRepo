#/bin/bash
docker ps --format "127.0.0.1 {{.Names}}.docker" >> /etc/hosts
