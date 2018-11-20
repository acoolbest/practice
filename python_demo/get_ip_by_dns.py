#!/usr/bin/env python
# -*- coding:utf8 -*-

import socket
import os
import sys
import re

def reply_to_iplist(data):
	assert isinstance(data, basestring)
	iplist = ['.'.join(str(ord(x)) for x in s) for s in re.findall('\xc0.\x00\x01\x00\x01.{6}(.{4})', data) if all(ord(x) <= 255 for x in s)]
	return iplist

def domain_to_ip(dnsserver,domain):
	dnsserver = dnsserver
	seqid = os.urandom(2)
	print "seqid~~~~~~~~~~~~"
	print seqid
	print "~~~~~~~~~~~~"
	host = ''.join(chr(len(x))+x for x in domain.split('.'))
	print "print x~~~~~~~~~~~~"
	for x in domain.split('.'):
		print x
		print len(x)
		print chr(len(x))+x
	print "host~~~~~~~~~~~~"
	print host
	print "~~~~~~~~~~~~"
	data = '%s\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00%s\x00\x00\x01\x00\x01' % (seqid, host)
	sock = socket.socket(socket.AF_INET,type=socket.SOCK_DGRAM)
	sock.settimeout(None)
	print "data~~~~~~~~~~~~"
	print data
	print "~~~~~~~~~~~~"
	sock.sendto(data, (dnsserver, 53))
	data = sock.recv(512)
	print "data~~~~~~~~~~~~"
	print data
	return reply_to_iplist(data)

#dnsServer = "114.114.114.114"
#dnsServer = "218.30.118.6"
dnsServer = "8.8.8.8"

def main(argv):
	if len(argv)!=2:
		print('please follow a host name! \neg:python dns_client.py baidu.com')
	else:
		p = domain_to_ip(dnsServer,argv[1])
		print "the ip address of "+argv[1]+" are as follows"
		print p
if __name__ == '__main__':
	main(sys.argv)