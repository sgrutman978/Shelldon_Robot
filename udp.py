import socket 

localIP     = "0.0.0.0"
localPort   = 3002
bufferSize  = 4096

msgFromServer       = "Hello UDP Client"
bytesToSend         = str.encode(msgFromServer)

# Create a datagram socket

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM) 

# Bind to address and ip

UDPServerSocket.bind((localIP, localPort))

print("UDP server up and listening")

# Listen for incoming datagrams

while(True):

    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)

    message = bytesAddressPair[0]

    address = bytesAddressPair[1]
    #print("/n/n/n/n")
    #i = 0
    #while (i < len(message)):
    #    print(message[i])
    #    i += 1

    res = ''.join(format(x, '02x') for x in message)
 
# printing result
    print("The string after conversion : " + str(res))

#    with open("my_file.jpg", "wb") as binary_file:
        # Write bytes to file
#        binary_file.write(message)

    clientMsg = "Message from Client:{}".format(message)
    clientIP  = "Client IP Address:{}".format(address)
    
    print(clientMsg)
    print(clientIP)

   

    # Sending a reply to client

    #UDPServerSocket.sendto(bytesToSend, address)
