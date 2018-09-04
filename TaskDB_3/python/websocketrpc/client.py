# ======================================================================================
# WebSocket based RPC client implementation
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
import urllib.request, logging, xmlrpc, jsonrpc


class Proxy:
  class WebSocketTransport:
    def __init__(self, proto, verbose, sslopt=None):
      self.ws = None
      self.proto = proto
      self.sslopt = sslopt
      self.verbose = verbose

    def request(self, host, handler, request_body, verbose=0):
      sock = self.make_connection(host, handler)
      nBytes = sock.send(request_body)
      p, u = self.getparser()
      data = sock.recv()
      if data:
        if self.verbose:
          logging.info('Websocket: Body='+str(repr(data)))
        p.feed(data)
      p.close()
      return u.close()

    def make_connection(self, host, handler):
      if not self.ws:
        import websocket
        self.ws = websocket.WebSocket(sslopt=self.sslopt)
      if not self.ws.connected:
        uri = self.proto+'://'+host+handler
        if self.verbose:
          logging.info('Websocket: URI='+uri)
        self.ws.connect(uri)
      return self.ws

  class ServerProxy:
    def __init__(self, uri, transport, encoding=None, verbose=0, version=None):
      if not version:
        version = jsonrpc.config.version
      self.__version = version
      req = urllib.request.Request(uri)
      schema = req.type
      self.uri = uri
      if schema not in ('ws','wss'):
        raise IOError('Unsupported JSON-RPC protocol: '+schema)
      self.__host      = req.host
      self.__handler   = req.selector
      self.__transport = transport
      self.__encoding  = encoding
      self.__verbose   = verbose

class JSONProxy:
  class WebSocketJsonTransport(Proxy.WebSocketTransport,jsonrpc.client.Transport):
    def __init__(self, proto, verbose = False):
      Proxy.WebSocketTransport.__init__(self,proto, verbose=verbose, sslopt=None)
      jsonrpc.client.Transport.__init__(self)

  class WebSocketJsonSafeTransport(Proxy.WebSocketTransport,jsonrpc.client.SafeTransport):
    def __init__(self, proto, verbose = False, sslopt=None):
      Proxy.WebSocketTransport.__init__(self,proto, verbose=verbose, sslopt=sslopt)
      jsonrpc.client.SafeTransport.__init__(self)

  class ServerProxy(Proxy.ServerProxy,jsonrpc.client.ServerProxy):
    def __init__(self, uri, transport=None, encoding=None, verbose=0, version=None, sslopt=None):
      if transport is None:
        req = urllib.request.Request(uri)
        schema = req.type
        if schema == 'ws':
          transport = JSONProxy.WebSocketJsonTransport(schema, verbose=verbose)
        elif schema == 'wss':
          transport = JSONProxy.WebSocketJsonSafeTransport(schema, verbose=verbose,sslopt=sslopt)
        else:
          raise Exception('Unsupported protocol type: '+schema)
      
      Proxy.ServerProxy.__init__(self,uri,transport,encoding,verbose,version)

class XMLProxy:
  class WebSocketXMLTransport(Proxy.WebSocketTransport,xmlrpc.client.Transport):
    def __init__(self, proto, verbose = False):
      Proxy.WebSocketTransport.__init__(self,proto,verbose)
      xmlrpc.client.Transport.__init__(self)

  class WebSocketXMLSafeTransport(Proxy.WebSocketTransport,xmlrpc.client.SafeTransport):
    def __init__(self, proto, verbose = False, sslopt=None):
      Proxy.WebSocketTransport.__init__(self,proto, verbose=verbose, sslopt=sslopt)
      xmlrpc.client.Transport.__init__(self)

  class ServerProxy(Proxy.ServerProxy,xmlrpc.client.ServerProxy):
    def __init__(self, uri, transport=None, encoding='utf-8', verbose=0, version=None, sslopt=None):
      if transport is None:
        req = urllib.request.Request(uri)
        schema = req.type
        if schema == 'ws':
          transport = XMLProxy.WebSocketXMLTransport(schema, verbose=verbose)
        elif schema == 'wss':
          transport = XMLProxy.WebSocketXMLSafeTransport(schema, verbose=verbose, sslopt=sslopt)
        else:
          raise Exception('Unsupported protocol type: '+schema)
      
      Proxy.ServerProxy.__init__(self,uri,transport,encoding,verbose,version)

XmlServerProxy  = XMLProxy.ServerProxy
JsonServerProxy = JSONProxy.ServerProxy
