# ======================================================================================
# Package initializing class
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------

from API import *
from CLI import Client as CLI

def connect(type, address, debug=False):
    type = type.lower()
    if type.find('direct') >= 0:
        return TaskDB(Connection(database=address), debug)
    elif address[:4] == 'http' and type.find('xmlrpc') >= 0:
        import xmlrpclib as rpclib
        return rpclib.ServerProxy(address)
    elif address[:4] == 'http' and type.find('jsonrpc') >= 0:
        import jsonrpclib as rpclib
        return rpclib.Server(address)
    elif type.find('cli') >= 0:
        if address[:4] == 'http' and address.lower().find('xmlrpc')>=0:
            return CLI('xmlrpc', address, debug)
        elif address[:4] == 'http' and address.lower().find('jsonrpc')>=0:
            return CLI('jsonrpc', address, debug)
        else:
            return CLI('direct', address, debug)
