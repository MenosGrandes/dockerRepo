set CONTAINER_NAME=%1
set DOCKERFILE_PATH=%2

docker build --rm -t %CONTAINER_NAME% %DOCKERFILE_PATH%
