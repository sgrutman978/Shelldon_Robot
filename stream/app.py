import io
import binascii
import socket
import threading
from flask import Flask, Response, render_template

UDP_IP = '0.0.0.0'  # Replace with your UDP server IP address
UDP_PORT = 3004  # Replace with your UDP server port

app = Flask(__name__)
udp_socket = None
image_data = b''

def start_udp_listener():
    global udp_socket, image_data
    #with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as udp_socket:
    #    udp_socket.bind((UDP_IP, UDP_PORT))
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind((UDP_IP, UDP_PORT))
    print("queen")
    while True:
        # Assuming the data received is in hexadecimal format
        bytesAddressPair = udp_socket.recvfrom(4096)
        message = bytesAddressPair[0]
        address = bytesAddressPair[1]
        res = ''.join(format(x, '02x') for x in message)
        new_image_data = binascii.unhexlify(res)
        image_data = new_image_data


@app.route('/')
def index():
    return render_template('index.html')

@app.route('/startUDP')
def startUDP():
    start_udp_listener()
    print("hhhhh")
    return


@app.route('/stream')
def stream():
    def generate():
        while True:
            frame = image_data

            # Create an in-memory stream for the frame
            stream = io.BytesIO()
            stream.write(frame)
            stream.seek(0)

            # Yield the frame as bytes
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + stream.read() + b'\r\n')

    return Response(generate(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    #udp_listener_thread = threading.Thread(target=start_udp_listener)
    #udp_listener_thread.daemon = True
    #udp_listener_thread.start()
    app.run(host="0.0.0.0", port=6969,debug=True)
