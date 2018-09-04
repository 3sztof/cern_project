#
#    Simple ssl socket server
#
import ssl, socket

context = ssl.SSLContext(ssl.PROTOCOL_TLSv1)
context.load_cert_chain(certfile="./server.crt",
                        keyfile='./server.key')

bindsocket = socket.socket()
bindsocket.bind(('localhost', 8085))
bindsocket.listen(5)

while True:
    newsocket, fromaddr = bindsocket.accept()
    sslsoc = context.wrap_socket(newsocket,server_side=True)
    print 'Address: ', fromaddr
    try:
        while True:
            request = sslsoc.read()
            if not request:
                print 'Connection closed.....'
                sslsoc.close()
                break
            print request
    except Exception, X:
        print 'Exception .... ',str(X)
