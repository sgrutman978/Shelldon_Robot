
from flask import render_template, Flask, request
import socket
import time
import sys

app = Flask(__name__)

UDP_IP = ""
UDP_PORT = 8888

@app.route("/")
def init_dummy():
    return "stevengrutman.com is a work in progress"


@app.route("/shelldon")
def shelldon():
    return render_template('home.html')

