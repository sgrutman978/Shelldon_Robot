
from flask import Response, Request, render_template, Flask, request
import socket
import time
import sys
import os
from camera_opencv import Camera 

app = Flask(__name__)

@app.route("/")
def init_dummy():
    return "stevengrutman.com is a work in progress"


@app.route("/shelldon")
def shelldon():
    return render_template('home.html')

@app.route("/please")
def please():
    return "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-len: " + String(len) + "\r\n\r\n"

@app.route('/camera')
def index():
    """Video streaming home page."""
    return render_template('my_file.jpg')


def gen(camera):
    """Video streaming generator function."""
    yield b'--frame\r\n'
    while True:
        frame = camera.get_frame()
        yield b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n--frame\r\n'


@app.route('/video_feed')
def video_feed():
    """Video streaming route. Put this in the src attribute of an img tag."""
    return Response(gen(Camera()),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

