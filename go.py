import sys
import os

print("Num Args given: " + str(len(sys.argv)))
print("Args: ", sys.argv);

cmd1 = "./csim"
#cmd2 = ""
cmd3 = " -s 2 -E 2 -b 4 -t ./traces/"
cmd4 = ""
both = False

s = 2
E = 2
b = 4

for i in range(len(sys.argv)):
	arg = sys.argv[i]
	#if arg == "-ref":
	#	cmd2 = "-ref"
	if arg == "-t":
		cmd4 = sys.argv[i+1]
	if arg == "-both":
		both = True
	if arg == "-a":
		s = sys.argv[i+1]
		E = sys.argv[i+2]
		b = sys.argv[i+3]


#cmd = cmd1 + cmd2 + cmd3 + cmd4
#def run(is_ref):
#	cargs = " -s %d -E %d -b %d -t %s" % 
#	cmd = cmd1 + is_ref + 	

args = ""
for arg in sys.argv[1:]:
	args += " " + arg

cmd1 = "./csim"

os.system(cmd1 + args)
os.system(cmd1 + "-ref" + args) 























