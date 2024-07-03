
docker compose  -f ../common/common-services.yml -f ../common/download_volume.yml -f production-compose.yml --env-file ".env" config > docker-compose.yaml
