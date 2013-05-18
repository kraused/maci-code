#!/usr/bin/env python

import optparse
import sys
import datetime
import json
import re
import types
import random
import os
import pprint
import shutil
import math
import subprocess


# ========================================
# Read a file
# ========================================
def read(filename):
	# The python-json library we use can only handle
	# one JSON object at a time. Hence, we need first to
	# split the input file into the different (top-level
	# objects -- we don't want to split for objects which are
	# part of other objects) strings
	def split(string):
		spl = []

		lvl = 0
		str = ""

		for c in string:
			str = str + c

			if "{" == c:
				lvl = lvl + 1
			if "}" == c:
				lvl = lvl - 1

				if 0 == lvl:
					spl.append(str)
					str = ""

		return spl


	lines = open(filename, "r").read().split("\n")

	# Remove all the comments
	str   = ""
	for line in lines:
		str += re.sub("#.*$", "", line)

	lst = []
	[lst.append(json.read(s)) for s in split(str) ]

	return lst


# ========================================
# Find in an array of dictionary
# ========================================
def find(dicts, key, val):
	for dict in dicts:
		if dict[key] == val:
			return dict

	return None


# ========================================
# String processing of input
# ========================================
def process(gvals, D): 
	# Recursive replacement function
	def replace(D, k, v):
		if   isinstance(D, str):
			while 1:
				t = re.search("%" + k + "(\s|$|[-+!?*;,/]|\)|\()", D)
				if None != t:
					if   '*' == t.group(1):
						D = re.sub("%" + k + t.group(1), str(v), D)
					elif ')' == t.group(1):
						D = re.sub("%" + k + "\)", str(v) + ")", D)
					elif '(' == t.group(1):
						D = re.sub("%" + k + "\(", str(v) + "(", D)
					else:
						D = re.sub("%" + k + t.group(1), str(v) + t.group(1), D)
				else:
					break

			D = D.replace("\t", "\\t")
			D = D.replace("\n", "\\n")

			return D
		elif isinstance(D, dict):
			Dp = {}
			for l, w in D.items():
				Dp[l] = replace(w, k, v)
			return Dp
		elif isinstance(D, list):
			Dp = []
			[ Dp.append(replace(w, k, v)) for w in D ]
			return Dp
		else:
			return D	

	# Function to evaluate mathematical formulas
	# between two !'s
	def compute(D):
		if   isinstance(D, str):
			Dp = ""
			i  = 0

			while 1:
				start = i
				for i in range(i,len(D)):
					if "!" == D[i]:
						for ip, cp in enumerate(D[i+1:]):
							if "!" == cp:
								ip = i + ip + 1
								break

						Dp += "%d" % eval(D[i+1:ip])

						i = ip + 1
						break
					else:
						Dp += D[i]

				if i >= len(D)-1:
					break
			return Dp
		elif isinstance(D, dict):
			Dp = {}
			for l, w in D.items():
				Dp[l] = compute(w)
			return Dp
		elif isinstance(D, list):
			Dp = []
			[ Dp.append(compute(w)) for w in D ]
			return Dp
		else:
			return D


	for k, v in gvals.items():
		D = replace(D, k, v)

	for k, v in D.items():
		D = compute(D)

	return D


# ========================================
# Create the output directory for the experiment.
# The directory is guaranteed to be unique
# ========================================
def createdir(app):
	odir = app["output"]
	# Make the name unique by appending a
	# random number.
	# Note that this is not totally bullet-proof
	# since the os.path.exists and os.makedirs
	# combination is not atomic
	while True == os.path.exists(odir):
		odir += random.sample("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", 1)[0]
	os.makedirs(odir)

	return odir


# ========================================
# Copy the content of the input folder to the
# output folder
# ========================================
def copyio(idir, odir, logfo):
	cps = {}

	for v in os.listdir(idir):
		if os.path.isfile(idir + "/" + v):
			shutil.copy(idir + "/" + v, odir + "/" + v)
			cps[idir + "/" + v] = odir + "/" + v

	logfo.write("\n COPIES PERFORMED:\n")
	pprint.pprint(cps, logfo)


# ========================================
# Return the submission script as a string
# ========================================
def createscript(host, app, exe, wdir, gvals, filename):
	str = ""

	str += "#!/bin/bash\n"
	str += "\n"

	for m in app["modules"]:
		str += "module load %s\n" % m

	str += "\n"
	str += "cd %s" % wdir

	str += "\n"
        str += "echo \"\" >> logfile.txt\n"
	str += "echo \" JOBID:\" >> logfile.txt\n"
	str += "echo $PBS_JOBID >> logfile.txt\n"
	str += "echo \"\" >> logfile.txt\n"

	# Create the hostfile
	str += "\n"
	str += app["mpi"]["nodes"] + "\n"

	str += "\n"
	str += "echo \"\" >> logfile.txt\n"
	str += "echo \" HOSTFILE:\" >> logfile.txt\n"
	str += "cat %s >> logfile.txt\n" % gvals["nodes"]
	str += "echo \"\" >> logfile.txt\n"
	
	str += "\n"
	str += "t0=`date +%s`\n"

	str += "\n"
        str += "echo \"\" >> logfile.txt\n"
	str += "echo \" START TIME:\" >> logfile.txt\n"
	str += "date >> logfile.txt\n"

	str += "\n"
	str += "echo \"\" >> logfile.txt\n"
	str += "echo \" STDOUT & STDERR:\" >> logfile.txt\n"
	str += app["mpi"]["cmd"] + " 2>&1 >> logfile.txt\n"

	str += "\n"
	str += "T=`echo $?`\n"
        str += "echo \"\" >> logfile.txt\n"
	str += "echo \" MPI RETURN VALUE:\" >> logfile.txt\n"
	str += "echo $T >> logfile.txt\n"
	
	str += "\n"
	str += "t1=`date +%s`\n"

	str += "\n"
	str += "echo \"\" >> logfile.txt\n"
	str += "echo \" EXECUTION TIME:\" >> logfile.txt\n"
	str += "dt=`echo \"$t1 - $t0\" | bc`\n"
	str += "echo \"$dt sec\" >> logfile.txt\n"

	# Fast indicator for success or failure
	str += "\n"
	str += "test \"0 == $T\" && touch OK\n"
	str += "test \"0 != $T\" && touch FAIL\n"
	
	str += "\n"
	str += "echo \"\" >> logfile.txt\n"

	return str


# ========================================
# Submit to the batch system
# ========================================
def qsub(host, script):
	subprocess.Popen(host["batch"]["sub"].split(" "), stdout = subprocess.PIPE).communicate()[0]


parser = optparse.OptionParser()
parser.add_option("-n", 
		"--npes",
		dest    = "npes",
		help    = "Number of MPI processing elements",
		type    = "int",
		default = 1)
parser.add_option("-t", 
		"--nthreads",
		dest    = "nthreads",
		help    = "Number of threads per processing element",
		type    = "int",
		default = 1)
parser.add_option("--host",
		dest    = "host",
		help    = "Host description file (JSON format)",
		type    = "string")
parser.add_option("-a",
		"--app",
		dest    = "app",
		help    = "Application description file (JSON format)",
		type    = "string",
		default = "maci.json")
parser.add_option("-x",
		"--exp",
		dest	= "exp",
		help	= "Experiments description file (JSON format)",
		type	= "string",
		default = "exp.json")
parser.add_option("-f",
		"--fake",
		action  = "store_true",
		dest    = "fake",
		default = False)

(opts, args) = parser.parse_args()

# The only argument is the experiment name
if 1 != len(args):
	parser.error("Need exactly one argument.")


# Read the application config
A = read(opts.app)[0]

# Read the host config
if None == opts.host:
	H = read(A["host"])[0]
else:
	H = read(opts.host)[0]

# Find the experiment to perform
E = find(read(opts.exp), "name", args[0])
if None == E:
	sys.exit(" Could not find experiment \"%s\"." % args[0])

# Compute the time
now = datetime.datetime.now()

# Store some global variables
gvals = {	"npes"     : E["npes"],
		"nthreads" : E["nthreads"],
		"wtime"    : E["wtime"],
		"user"     : A["user"],
		"name"     : E["name"],
		"exe"      : A["exe"],
		"args"	   : " ".join(E["args"]),
		"a"	   : now.strftime("%a"),
		"b"	   : now.strftime("%b"),
		"y"	   : now.strftime("%y"),
		"H"	   : now.strftime("%H"),
		"M"	   : now.strftime("%M"),
		"S"	   : now.strftime("%S"),
		"nodes"	   : "hosts.txt",
		"script"   : "job.SC" }

H = process(gvals, H)
A = process(gvals, A)
E = process(gvals, E)

# Check the restrictions
for r in H["batch"]["restr"]:
	if '1' != r.strip():
		sys.exit(" Not all batch system restrictions are fulfilled. Cannot submit the job.")

# Replace relative paths by absolute paths
A["exe"]    = os.path.abspath(A["exe"])
A["output"] = os.path.abspath(A["output"])
E["input"]  = os.path.abspath(E["input"])

# Create the job directory
odir = createdir(A)

# Change into the output directory
os.chdir(odir)

# Open the logfile
logfo = open("logfile.txt", "w")

logfo.write("\n HOST:\n")
pprint.pprint(H, logfo)

logfo.write("\n APPLICATION:\n")
pprint.pprint(A, logfo)

logfo.write("\n EXPERIMENT:\n")
pprint.pprint(E, logfo)

# Copy the content of the input directory to the output directory
copyio(E["input"], odir, logfo)

# Create the submission script as a string
sc = createscript(H, A, E, odir, gvals, gvals["script"])

logfo.write("\n SUBMIT SCRIPT:\n")
logfo.write("%s =\n\"\n%s\"\n" % (gvals["script"], sc))

# Write it to a file
open(gvals["script"],"w").write(sc)

# Submit it
if not opts.fake:
	qsub(H, gvals["script"])

print " DONE."

