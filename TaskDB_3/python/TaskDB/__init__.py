# ======================================================================================
# Package initializing class
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------

from API import *
from CLI import Client as CLI

def _get_ssl_config(sslopt):
    #
    # Note: This requires that the environment
    # WEBSOCKET_CLIENT_CA_BUNDLE=`pwd`/ssl/server.crt
    # points to the correct server certificate file!
    #
    if sslopt is None:
        sslopt={}
    if not sslopt.get('load_default_certs', None):
        sslopt['load_default_certs'] = True
    if not sslopt.get('load_default_certs', None):
        sslopt['load_default_certs'] = True
    #if not sslopt.get('ca_certs', None):
    #    sslopt['ca_certs'] = './ssl/server.crt'
    ##print 'SSL Connection Options:',str(sslopt)
    return sslopt

def _create_ssl_context(sslopt):
    import ssl
    sslopt  = _get_ssl_config(sslopt)
    print('SSL Connection Options:',str(sslopt))
    context = ssl.SSLContext(ssl.PROTOCOL_SSLv23)
    context.verify_mode    = ssl.CERT_REQUIRED
    context.check_hostname = sslopt.get('check_hostname',None) is not None
    ##context.verify_mode    = sslopt['cert_reqs']
    if 'load_default_certs' in sslopt:
        context.load_default_certs()
    if 'ciphers' in sslopt:
        context.set_ciphers(sslopt['ciphers'])
    if 'cert_chain' in sslopt:
        certfile, keyfile, password = sslopt['cert_chain']
        context.load_cert_chain(certfile, keyfile, password)
    if 'ecdh_curve' in sslopt:
        context.set_ecdh_curve(sslopt['ecdh_curve'])
    context.load_verify_locations(cafile=sslopt['ca_certs'])
    context.load_default_certs(ssl.Purpose.SERVER_AUTH)
    return context

def connect(type, address, sslopt=None, debug=False):
    #import pdb; pdb.set_trace()
    type = type.lower()
    if type.find('direct') >= 0:
        return TaskDB(Connection(database=address), debug)
    elif address[:5] == 'http:'  and address.lower().find('json') >= 0:
        import jsonrpc.client as rpclib
        return rpclib.ServerProxy(address)
    elif address[:6] == 'https:' and address.lower().find('json') >= 0:
        # The guy implementing jsonrpc did not keep interfaces properly. Does not work!
        import jsonrpc.client as rpclib
        return rpclib.ServerProxy(address,context=_create_ssl_context(sslopt))
    elif address[:3] == 'ws:'    and address.lower().find('json') >= 0:
        import websocketrpc.client as rpclib
        return rpclib.JsonServerProxy(address)
    elif address[:4] == 'wss:'   and address.lower().find('json') >= 0:
        import websocketrpc.client as rpclib
        sslopt = _get_ssl_config(sslopt)
        return rpclib.JsonServerProxy(address,sslopt=sslopt)

    elif address[:5] == 'http:'  and address.lower().find('xml') >= 0:
        import xmlrpc.client as rpclib
        return rpclib.ServerProxy(address)
    elif address[:6] == 'https:' and address.lower().find('xml') >= 0:
        import xmlrpc.client as rpclib
        return rpclib.ServerProxy(address,context=_create_ssl_context(sslopt))
    elif address[:3] == 'ws:'    and address.lower().find('xml') >= 0:
        import websocketrpc.client as rpclib
        return rpclib.XmlServerProxy(address)
    elif address[:4] == 'wss:'   and address.lower().find('xml') >= 0:
        import websocketrpc.client as rpclib
        sslopt = _get_ssl_config(sslopt)
        return rpclib.XmlServerProxy(address,sslopt=sslopt)

    elif type.find('cli') >= 0:
        if address[:4] == 'http' and address.lower().find('xmlrpc')>=0:
            return CLI('xmlrpc', address, debug)
        elif address[:4] == 'http' and address.lower().find('jsonrpc')>=0:
            return CLI('jsonrpc', address, debug)
        else:
            return CLI('direct', address, debug)
    raise Exception('TaskDB: Unknown protocol connecteor requested: type="%s" address="%s"'%(type, str(address),))
