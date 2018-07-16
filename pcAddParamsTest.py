#!/usr/bin/env python

default_params = '-K -M 5 -X 300 -g onliners -p -n online'
split_default_params = default_params.split()

l = len(split_default_params)

dic = {}
for i in range(0, l - 1):
    if split_default_params[i].find('-') != -1:
        # This is a parameter name - key, we should check if it is not empty - check the next one
        if split_default_params[i + 1].find('-') != -1:
            # It is empty, append '' to it
            dic[split_default_params[i]] = ''
        # It was not empty, assign the next value to it.
        else:
            dic[split_default_params[i]] = split_default_params[i + 1]


print dic
res = ''
for k,v in dic.items():
    res = res + ' ' + k + ' ' + v

print 'To string:', res

str = ''

for i in dic:
    str += i
