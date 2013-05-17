
import json
import re
import subprocess
import os
import datetime

# Hmm... That's not nice. Can we can get around importing
# all this stuff?
from Host   	import Host
from Walker	import Walker
from Job	import Job
from Test	import Test
from read   	import read_JSON

# Env: Contains information about the environment
#      such as the target and source directories
class Env:

	def __init__(self, filename):
		self.__dict__["table"] = {	# The host file
						"host"    	: "",
						# The name of the environment (what are
						# we doing here)
						"name"	        : "",
						# Mail address of the responsible person
						"responsible" 	: "",
						# The input directory
						"input"   	: "",
						# The output directory
						"output"  	: "",
						# The path to the executable
						"exe"     	: "",
						# The configuration of the executable
						"conf"    	: [],
						# List of modules to load before
						# the executable can run 
						"modules" 	: [],
						# Maximal time for testing (not for the
						# individual tests)
						"max_walltime"  : 0,
						# The MPI infos. Note that these
						# are not part of the host data because
						# different executables might be
						# compiled with different MPI versions
						"mpi" : {       # The implementation
								"type" : "",
								# The execution command (you know, mpiexec is not standard)
								"cmd"  : "" } }

		t = json.read(read_JSON(filename))

		for k, v in json.read(read_JSON(filename)).items():
			if "type" == k:
				assert "env" == v
			self.table[k] = v

                for k, v in t["mpi"].items():
			self.table["mpi"][k] = v

		now = datetime.datetime.now()
		for v in [ "%a", "%b", "%y", "%H", "%M" ]:
			self.output = re.sub(v, now.strftime(v), self.output)

		# Save the current time
		self.__dict__["now"] = now.strftime("%a-%b-%y-%H-%M")

	def __getattr__(self, k):
		return self.table[k]

	def __setattr__(self, k, v):
		self.table[k] = v

	def __repr__(self):
		return repr(self.table)

	def __str__(self):
		return str(self.table)

