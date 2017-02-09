import os
import sys
import subprocess

cmd = "./csim"
cmdstring = " -s {} -E {} -b {} -t "

fname = sys.argv[1]
a = sys.argv[2]
b = sys.argv[3]
c = sys.argv[4]

#print(sys.argv)

def parseNums(first):
	splitted = first.split(':')[1:]
	more = map(lambda x: x.split(" ")[0], splitted)
	more = map(lambda x: x.split("\n")[0], more)
	try:
		more = map(int, more)
	except:
		pass
	return list(more)	

def do(a, b, c, fname, cmdstring):
	cmdstring = cmd + cmdstring.format(a, b, c)
	cmdstring += fname + " > tempbuff"
	print(cmdstring)
	cmdstring1 = cmd + cmdstring
	cmdstring2 = cmd + "-ref" + cmdstring

	#print(cmdstring1, cmdstring2)
	
	tempbuff = open("tempbuff",'r')
	os.system(cmdstring1)
	first = tempbuff.read()
	tempbuff.close()
	tempbuff = open("tempbuff", 'r')
	os.system(cmdstring2)
	second = tempbuff.read()
	print(first, second)
	tempbuff.close()

	

	first = parseNums(first)
	second = parseNums(second)

	for i in range(len(first)):
		if first[i] != second[i]:
			print("Error! " + a + b + c)
			return
	print("All goood: " + str(a) + " " +  str(b) + " " + str(c))	

import time


#for i in range(1,int(a)):
#	for j in range(1,int(b)):
#		#for k in range(1,int(c)):
#		do(i, j, 1, fname, cmdstring)
#		time.sleep(.1)
do(a, b, c, fname, cmdstring)


