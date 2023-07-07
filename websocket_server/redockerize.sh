docker stop dropin2
docker build . -t dropin2
docker run -p 56112:56112 -d dropin2
docker ps
