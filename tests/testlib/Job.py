
import re

# Job: Represents a batch job
class Job:

	# Create a new job
	def __init__(self, name, npes, walltime, exe, arguments, workdir):
		self.__dict__["table"] = {	# The job name
						"name" : name,
						# The number of processing elements
						"npes" : npes,
						# The walltime
						"walltime" : walltime,
						# The executable
						"exe" : exe,
						# The arguments
						"arguments" : arguments,
						# The job id
						"id" : "",
						# The working directory in which
						# the job should run
						"workdir" : workdir }

	def __getattr__(self, k):
		return self.table[k]

	def __setattr__(self, k, v):
		self.table[k] = v

	# Write the job execution script.
	def write(self, host, env):
		fo = open(self.name + ".sh", "w")

		fo.write("#!/bin/bash\n")
		fo.write("\n")

		[ fo.write("module load %s\n" % m) for m in env.modules ]
		fo.write("\n")

		fo.write("test -d %s || mkdir -p %s\n" % (self.workdir, self.workdir))
		fo.write("\n")

		fo.write("cd %s\n" % self.workdir)
		fo.write("\n")

		cmd = env.mpi["cmd"]
		cmd = re.sub("%npes", "%d" % self.npes, cmd)
		cmd = re.sub("%exe" , self.exe        , cmd)
		cmd = re.sub("%args", self.arguments  , cmd)
		fo.write(cmd + "\n")

		fo.close()

		return self.name + ".sh"

	def __repr__(self):
		return repr(self.table)

	def __str__(self):
		return str(self.table)

	def __eq__(self, other):
		return self.table == other.table

