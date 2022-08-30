import socket
import queue
import cv2
import time
import threading
import errno

HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
PORT = 65433 # Port to listen on (non-privileged ports are > 1023)
queue = queue.Queue()

class tcp_server(socket.socket):
    clients = []
    frame = None
    port = 0

    def __init__(self, target_port=1234):
        socket.socket.__init__(self)
        #To silence- address occupied!!
        self.port = target_port

        self.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.bind(('0.0.0.0', self.port))
        self.listen(5)

    def run(self):
        print("Server started on port {}".format(self.port))
        try:
            self.accept_clients()
        except Exception as ex:
            print(ex)
        finally:
            print("Server closed")
            for client in self.clients:
                client.close()
            self.close()

    def accept_clients(self):
        while 1:
            (clientsocket, address) = self.accept()
            #Adding client to clients list
            self.clients.append(clientsocket)
            #Client Connected
            self.onopen(clientsocket)
            #Receiving data from client
            #thread.start_new_thread(self.recieve, (clientsocket,))
            threading.Thread(target=self.recieve, args=(clientsocket,)).start()

    def recieve(self, client):
        while 1:
            try:
                data = client.recv(1024)
                if data == '':
                    break
                #Message Received
                self.onmessage(client, data)
            except socket.error as e:
                if e.errno == errno.ECONNRESET:
                    # Handle disconnection -- close & reopen socket etc.
                    break
                else:
                    # Other error, re-raise
                    break
        #Removing client from clients list
        self.clients.remove(client)
        #Client Disconnected
        self.onclose(client)
        #Closing connection with client
        client.close()
        #Closing thread
        return

    def broadcast(self, message):
        #Sending message to all clients
        for client in self.clients:
            client.send(message)

    def onopen(self, client):
        pass

    def onmessage(self, client, message):
        if self.frame is not None:

            width = self.frame.shape[1]
            height = self.frame.shape[0]

            client.sendall(b"\xAA\x55\xAA\x55")
            client.sendall((width*height*3 + 4).to_bytes(4, byteorder='little'))
            client.sendall(width.to_bytes(2, byteorder='little'))
            client.sendall(height.to_bytes(2, byteorder='little'))
            #print("Send frame")
            client.sendall(self.frame.tobytes())

    def onclose(self, client):
        pass

    def set_frame(self, frame):
        self.frame = frame

# class tcp_server:
#     frame = None
#
#     def set_frame(self, frame):
#         self.frame = frame
#
#     def run(self):
#
#         with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
#             s.bind((HOST, PORT))
#             s.listen()
#             conn, addr = s.accept()
#             with conn:
#                 print(f"Connected by {addr}")
#                 while True:
#                     data = conn.recv(1024)
#                     if not data:
#                         break
#
#                     if self.frame is not None:
#                         conn.sendall(b"\xAA\x55\xAA\x55")
#                         conn.sendall(self.frame.shape[1].to_bytes(2, byteorder='little'))
#                         conn.sendall(self.frame.shape[0].to_bytes(2, byteorder='little'))
#                         print("Send frame")
#                         conn.sendall(self.frame.tobytes())