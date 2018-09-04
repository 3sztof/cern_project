#
#    Simple ssl socket client
#
import ssl, time, socket

context = ssl.SSLContext(ssl.PROTOCOL_TLSv1)
context.verify_mode = ssl.CERT_REQUIRED
context.check_hostname = True
context.load_default_certs()
context.load_verify_locations(cafile="./server.crt")
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
ssl_sock = context.wrap_socket(s, server_hostname='localhost')
ssl_sock.connect(('localhost', 8085))

while True:
    ssl_sock.send('Hello world!')
    time.sleep(1)
