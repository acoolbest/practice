#!/usr/bin/env python
#---------------------------------------------
#print 1 + 2 * 4
#---------------------------------------------
#try:
#	filename = raw_input('Enter file name: ')
#	fobj = open(filename, 'r')
#	#fobj = open('/home/charge/share/practice/python_demo/test', 'r')
#	for eachLine in fobj:
#		print eachLine,
#except IOError, e:
#	print 'file open error:', e
#---------------------------------------------
#class FooClass(object):
#	"""my very first class: FooClass"""
#	version = 0.1 # class (data) attribute
#	def __init__(self, nm='John Doe'):
#		"""constructor"""
#		self.name = nm # class instance (data) attribute
#		print 'Created a class instance for', nm
#	def showname(self):
#		"""display instance attribute and class name"""
#		print 'Your name is', self.name
#		print 'My name is', self.__class__.__name__
#	def showver(self):
#		"""display class(static) attribute"""
#		print self.version # references FooClass.version
#	def addMe2Me(self, x): # does not use 'self'
#		"""apply + operation to argument"""
#		return x + x
#foo1 = FooClass()
#foo2 = FooClass('abc')
#foo1.showname()
#foo2.showname()
#foo1.showver()
#foo2.showver()
#print foo1.addMe2Me(5), foo1.addMe2Me('xyz')
#---------------------------------------------
#a = raw_input()
#print 'aaa is', a
#print 'int is %d' % int(a)
#if int(a)>0:
#	print ('a > 0')
#elif int(a)==0:
#	print ('a == 0')
#else:
#	print ('a < 0')
#---------------------------------------------
#i=0
#while i<=10:
#	print i
#	i += 1
#---------------------------------------------
#print(list(range(11)))
#print(list(range(0,11)))
#print(list(range(0,11,1)))
#a = list(range(11))
#for i in a:
#	print i
#---------------------------------------------
#a = raw_input()
#i=0
#while i<len(a):
#	print(a[i])
#	i+=1
#print ('-'*20)
#for i in a:
#	print(i)
#print ('-'*20)
#for i, ch in enumerate(a):
#	print(ch, i)
#print ('-'*20)
#for ch in enumerate(a):
#	print(ch)
#---------------------------------------------
#a = [1,2,3,4,5]
#b = (6,7,8,9,10)

#x=0
#for i in a:
#	x = x+i
#print(x)

#i=0
#x=0
#while i<len(b):
#	x = x+b[i]
#	i+=1
#print(x)
#
#x=0
#for i in range(len(a)):
#	a[i] = int(raw_input())
#	x+=a[i]
#	i+=1
#print(x)
#---------------------------------------------
#a = [1,2,3,4,6]
#x=0
#for i in a:
#	x = x+i
#print(x/len(a), float(x)/len(a)) 
#---------------------------------------------
#def foo():
#	while True:
#		a = raw_input()
#		if int(a) > 1 and int(a) < 100:
#			print('True, a is ' + a)
#			return True
#		else:
#			print('False, a is ' + a)
#foo()
#---------------------------------------------
print'''hi there, this is a long message for you
that goes over multiple lines... you will find
out soon that triple quotes in Python allows
this kind of fun! it is like a day on the beach!'''
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------
#---------------------------------------------