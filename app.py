
from flask import render_template, Flask, request
import socket
import time
import sys

app = Flask(__name__)

UDP_IP = ""
UDP_PORT = 8888

@app.route("/")
def init_dummy():
    return "Error"


@app.route("/ip")
@app.route('/ip/<ipInput>')
def ip(ipInput=None):
    UDP_IP = ipInput
    return render_template('home.html', ipInput=ipInput)


@app.route('/motorCommand', methods=['GET', 'POST'])
def motorCommand():
    if request.method == "POST":
        print(request.form["command"])
        return "good stuff"



def sendCommandToRobot(command):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    sock.sendto(bytes(f"{command}", "utf-8"), (UDP_IP, UDP_PORT))
