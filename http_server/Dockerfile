FROM python:3.6

WORKDIR /home/app

COPY requirements.txt requirements.txt
RUN pip3 install -r requirements.txt

COPY app.py /home/app

RUN mkdir -p templates
COPY templates/home.html /home/app/templates

CMD [ "python3", "-m" , "flask", "run", "--host=0.0.0.0", "--port=80"]
