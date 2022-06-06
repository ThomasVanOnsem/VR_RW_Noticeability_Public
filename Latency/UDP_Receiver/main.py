#

import socket

HOST = '192.168.255.216'
PORT = 5555


server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind((HOST, PORT))

print(f'Listening from address {HOST} on port {PORT}')

received_messages = 0
while True:
    message, address = server_socket.recvfrom(1024)
    print(f'({received_messages}) Message from {str(address)}: {message}')
    received_messages += 1

