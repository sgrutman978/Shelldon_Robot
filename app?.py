
from flask import render_template, Flask, request
import socket
import time
import sys

app = Flask(__name__)

udp_ip_global = "8"

@app.route("/")
def init_dummy():
    return "Error"


@app.get('/ip/<ipInput>')
def ip(ipInput):
    udp_ip_global = ipInput
    # print(udp_ip)
    return render_template('home.html', ipInput=ipInput)


@app.route('/motorCommand', methods=['GET', 'POST'])
def motorCommand():
    if request.method == "POST":
        print("ddd")
        print(request.form["command"])
        print("eee")
        sendCommandToRobot(request.form["command"], request.form["ipInput"])
        return "good stuff"



def sendCommandToRobot(command, udp_ip):
    print("aaa")
    print(udp_ip)
    print("bbb")
    udp_port = 8888
    print(udp_port)
    print("ccc")
    print(udp_ip_global)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    sock.sendto(bytes(f"{command}", "utf-8"), (udp_ip, udp_port))
