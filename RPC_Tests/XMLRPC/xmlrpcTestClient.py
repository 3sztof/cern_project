#!/usr/bin/env python
import xmlrpclib

s = xmlrpclib.ServerProxy('http://localhost:2137')
print s.pow(2,3)  # Returns 2**3 = 8
print s.add(2,3)  # Returns 5
print s.div(5,2)  # Returns 5//2 = 2

print 'TaskTest'
print s.selectTask('Task1')


# Print list of available methods
print s.system.listMethods()