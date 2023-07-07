docker build . -t flaskapp
docker run -p 80:80 -d flaskapp
docker ps
