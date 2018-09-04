'''
The MIT License (MIT)

Copyright (C) 2014, 2015 Seven Watt <info@sevenwatt.com>
<http://www.sevenwatt.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

import struct, base64, socket, hashlib, logging
from http.server import SimpleHTTPRequestHandler

'''
+-+-+-+-+-------+-+-------------+-------------------------------+
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                     Payload Data continued ...                |
+---------------------------------------------------------------+
'''

FIN    = 0x80
OPCODE = 0x0f
MASKED = 0x80
PAYLOAD_LEN = 0x7f
PAYLOAD_LEN_EXT16 = 0x7e
PAYLOAD_LEN_EXT64 = 0x7f

OPCODE_CONTINUATION = 0x0
OPCODE_TEXT         = 0x1
OPCODE_BINARY       = 0x2
OPCODE_CLOSE_CONN   = 0x8
OPCODE_PING         = 0x9
OPCODE_PONG         = 0xA
WS_GUID = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11'


class WebSocketError(Exception):
    pass

class HTTPWebSocketsHandler(SimpleHTTPRequestHandler):
    
    def on_ws_message(self, message):
        """Override this handler to process incoming websocket messages."""
        pass
        
    def on_ws_connected(self):
        """Override this handler."""
        pass
        
    def on_ws_closed(self):
        """Override this handler."""
        pass
        
    def send_message(self, message):
        self._send_message(OPCODE_TEXT, message)

    def setup(self):
        import threading
        SimpleHTTPRequestHandler.setup(self)
        self.mutex = threading.Lock()
        self.connected = False
                
    # def finish(self):
        # #needed when wfile is used, or when self.close_connection is not used
        # #
        # #catch errors in SimpleHTTPRequestHandler.finish() after socket disappeared
        # #due to loss of network connection
        # try:
            # SimpleHTTPRequestHandler.finish(self)
        # except (socket.error, TypeError) as err:
            # self.log_message("finish(): Exception: in SimpleHTTPRequestHandler.finish(): %s" % str(err.args))

    # def handle(self):
        # #needed when wfile is used, or when self.close_connection is not used
        # #
        # #catch errors in SimpleHTTPRequestHandler.handle() after socket disappeared
        # #due to loss of network connection
        # try:
            # SimpleHTTPRequestHandler.handle(self)
        # except (socket.error, TypeError) as err:
            # self.log_message("handle(): Exception: in SimpleHTTPRequestHandler.handle(): %s" % str(err.args))

    def checkAuthentication(self):
        auth = self.headers.get('Authorization')
        if auth != "Basic %s" % self.server.auth:
            self.send_response(401)
            self.send_header("WWW-Authenticate", 'Basic realm="Plugwise"')
            self.end_headers();
            return False
        return True
        
    def do_GET(self):
        logging.debug('Path: ' + str(self.path))
        logging.debug('Headers:'+str(self.headers))
        if hasattr(self.server,'auth') and self.server.auth and not self.checkAuthentication():
            logging.debug('Authentication:'+str(self.server.auth))
            return
        if self.headers.get("Upgrade", None) == "websocket":
            self._handshake()
            #This handler is in websocket mode now.
            #do_GET only returns after client close or socket error.
            self._read_messages()
        else:
            logging.warning('++ Invalid WebSocket GET call: Forward to SimpleHTTPRequestHandler')
            SimpleHTTPRequestHandler.do_GET(self)
                          
    def _read_messages(self):
        while self.connected == True:
            try:
                self._read_next_message()
            except (socket.error, WebSocketError) as e:
                #websocket content error, time-out or disconnect.
                self.log_message("RCV: Close connection: Socket Error %s" % str(e.args))
                self._ws_close()
            except Exception as err:
                #unexpected error in websocket connection.
                self.log_error("RCV: Exception: in _read_messages: %s" % str(err.args))
                self._ws_close()
        
    def _read_next_message(self):
        #self.rfile.read(n) is blocking.
        #it returns however immediately when the socket is closed.
        #import pdb; pdb.set_trace()
        #logging.info(self.path+' ++ Processing next WebSocket message.')
        try:
            self.opcode = ord(self.rfile.read(1)) & OPCODE
            length = ord(self.rfile.read(1)) & PAYLOAD_LEN
            if length == PAYLOAD_LEN_EXT16:
                length = struct.unpack(">H", self.rfile.read(2))[0]
            elif length == PAYLOAD_LEN_EXT64:
                length = struct.unpack(">Q", self.rfile.read(8))[0]
            masks = []
            masks.append(ord(self.rfile.read(1)) & 0xFF)
            masks.append(ord(self.rfile.read(1)) & 0xFF)
            masks.append(ord(self.rfile.read(1)) & 0xFF)
            masks.append(ord(self.rfile.read(1)) & 0xFF)
            decoded = ""
            while length > 0:
                decoded += chr(ord(self.rfile.read(1)) ^ masks[len(decoded) % 4])
                length = length - 1
            self._on_message(decoded)
        except (struct.error, TypeError) as e:
            #catch exceptions from ord() and struct.unpack()
            if self.connected:
                self.log_error("RCV: WebSocket error")
                raise WebSocketError("Websocket read aborted while listening")
            else:
                #the socket was closed while waiting for input
                self.log_error("RCV: _read_next_message aborted after closed connection")
                pass
        
    def _send_message(self, opcode, message):
        try:
            #use of self.wfile.write gives socket exception after socket is closed. Avoid.
            header = bytearray()
            header.append(ord(chr(MASKED + opcode)))
            length = len(message)
            if length <= 125:
                header.append(ord(chr(length)))
            elif length >= 126 and length <= 65535:
                header.append(ord(chr(PAYLOAD_LEN_EXT16)))
                header.extend(struct.pack(">H", length))
            else:
                header.append(ord(chr(PAYLOAD_LEN_EXT64)))
                header.extend(struct.pack(">Q", length))
            if length > 0:
                self.request.send(header + message)
        except socket.error as e:
            #websocket content error, time-out or disconnect.
            self.log_message("SND: Close connection: Socket Error %s" % str(e.args))
            self._ws_close()
        except Exception as err:
            #unexpected error in websocket connection.
            self.log_error("SND: Exception: in _send_message: %s" % str(err.args))
            self._ws_close()

    def _handshake(self):
        if self.headers.get("Upgrade", None) != "websocket":
            return
        self.originally_recieved_headers = self.headers
        if self.headers.get("Upgrade", None) != "websocket":
            return
        key    = self.headers['Sec-WebSocket-Key']
        digest = base64.b64encode(hashlib.sha1((key + WS_GUID).encode()).digest()).decode()
        response = 'HTTP/1.1 101 Switching Protocols\r\n'
        response += 'Upgrade: websocket\r\n'
        response += 'Connection: Upgrade\r\n'
        response += 'Sec-WebSocket-Accept: %s\r\n\r\n' % digest
        self.handshake_done = self.request.send(response.encode())
        self.connected = True
        #self.close_connection = 0
        self.on_ws_connected()
    
    def _ws_close(self):
        #avoid closing a single socket two time for send and receive.
        self.mutex.acquire()
        try:
            if self.connected:
                self.connected = False
                #Terminate BaseHTTPRequestHandler.handle() loop:
                self.close_connection = 1
                #send close and ignore exceptions. An error may already have occurred.
                try: 
                    self._send_close()
                except:
                    pass
                self.on_ws_closed()
            else:
                self.log_message("_ws_close websocket in closed state. Ignore.")
                pass
        finally:
            self.mutex.release()
            
    def _on_message(self, message):
        #self.log_message("_on_message: opcode: %02X msg: %s" % (self.opcode, message))
        
        # close
        if self.opcode == OPCODE_CLOSE_CONN:
            self.connected = False
            #Terminate BaseHTTPRequestHandler.handle() loop:
            self.close_connection = 1
            try:
                self._send_close()
            except:
                pass
            self.on_ws_closed()
        # ping
        elif self.opcode == OPCODE_PING:
            _send_message(OPCODE_PONG, message)
        # pong
        elif self.opcode == OPCODE_PONG:
            pass
        # data
        elif (self.opcode == OPCODE_CONTINUATION or 
              self.opcode == OPCODE_TEXT or 
              self.opcode == OPCODE_BINARY):
            self.on_ws_message(message)

    def _send_close(self):
        #Dedicated _send_close allows for catch all exception handling
        msg = bytearray()
        msg.append(FIN + OPCODE_CLOSE_CONN)
        msg.append(0x00)
        self.request.send(msg)
    
