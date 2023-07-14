import io
import socket
import binascii
from flask import Flask, Response, render_template

app = Flask(__name__)

localIP     = "0.0.0.0"
localPort   = 3001
bufferSize  = 4096
# Create a datagram socket
with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as udp_socket:
    # Bind to address and ip
    udp_socket.bind((localIP, localPort))


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/stream')
def stream():
    def generate():
        # Replace this with your logic to fetch and convert JPEG images
        # from hexadecimal format to binary
        while(True):
            bytesAddressPair, _ = udp_socket.recvfrom(4096)
            message = bytesAddressPair[0]
            address = bytesAddressPair[1]

            image_hex = ''.join(format(x, '02x') for x in message)
            print("a\n");
            print(image_hex);
            image_binary = binascii.unhexlify(image_hex)

        # Simulate streaming frames
#        while True:
            # Replace this with your logic to fetch and convert JPEG images
            # from hexadecimal format to binary
            frame = image_binary

            # Create an in-memory stream for the frame
            stream = io.BytesIO()
            stream.write(frame)
            stream.seek(0)

            # Yield the frame as bytes
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + stream.read() + b'\r\n')

    return Response(generate(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=6969, debug=True)








# Listen for incoming datagrams
#while(True):
#    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)
#    message = bytesAddressPair[0]
#    address = bytesAddressPair[1]
    #print("/n/n/n/n")
    #i = 0
    #while (i < len(message)):
    #    print(message[i])
    #    i += 1
#    res = ''.join(format(x, '02x') for x in message)
# printing result
#    print("The string after conversion : " + str(res))


#    UDPServerSocket.sendto(bytesToSend, address)
