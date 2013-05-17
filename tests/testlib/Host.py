
import json
import re
import subprocess
import os

from read import read_JSON

# Host: A representation of the information we
#       have about the machine we are working
#       on (like the queueing system, ...)
class Host:

	def __init__(self, filename):
		self.__dict__["table"] = {	# The name of the host. Typically the hostname
						"host"  : "anonymous",
						# The batch system description
						"batch" : {     # The type (PBS, SGE, LoadLeveler, ...)
								"type" : "",
								# The executables used to submit, delete and check
								# jobs
								"sub"  : "",
								"del"  : "",
								"stat" : "" },
						# Template for mailing
						"sendmail" : "" }

		t = json.read(read_JSON(filename))

		for k, v in t.items():
			if "type" == k:
				assert "host" == v
			self.table[k] = v

		for k, v in t["batch"].items():
			self.table["batch"][k] = v


	def __getattr__(self, k):
		return self.table[k]

	def __setattr__(self, k, v):
		self.table[k] = v

        def __repr__(self):
                return repr(self.table)

        def __str__(self):
                return str(self.table)

	# Submit a job to the queuing system and return the job id
	def submit(self, env, job):
		# First we need to write the submit script
		script = job.write(self, env)

		cmd = self.batch["sub"]
		# We need to replace the placeholder in the template
		cmd = re.sub("%npe"   , "%d" % job.npes, cmd)
		cmd = re.sub("%wtime" , job.walltime   , cmd)
		cmd = re.sub("%name"  , job.name       , cmd)
		cmd = re.sub("%script", script         , cmd)

		# Next we need to evaluate all math formulas in the
		# string
		cmd = self.eval_math_formulas(cmd)

		return subprocess.Popen(cmd.split(" "), stdout = subprocess.PIPE).communicate()[0]

	# Evaluate math stuff in a string. This function evaluates expressions
	# between $( and ). Recursion doesn't make sense and is hence forbidden.
	def eval_math_formulas(self, str):
		out = ""
		i   = 0

		while(True):
			start = i
			for i in range(start, len(str)):
				if "$" == str[i]:
					if "(" != str[i+1]:
						raise Exception("malformed string '%s'" % str)

					# Go forward to find the end of the expression
					lvl = 0
					for ip, cp in enumerate(str[i+1:]):
						if "(" == cp:
							lvl = lvl + 1
						if ")" == cp:
							lvl = lvl - 1
							if 0 == lvl:
								ip = i+ip+1
								break

					assert 0 == lvl

					out += "%d" % eval(str[i+2:ip])

					i = ip + 1
					break
				else:
					out += str[i]

			if i >= len(str)-1:
				break

		return out

	# Check if a job has finished
	def finished(self, job):
		cmd = re.sub("%jobid", job.id.strip(), self.batch["stat"]).strip() + " &> /dev/null"
		return 0 != os.system(cmd)

	# Delete a job
	def delete(self, job):
		subprocess.Popen(re.sub("\s+", " ", re.sub("%jobid", job.id, self.batch["del"])).strip().split(" "), stdout = subprocess.PIPE).communicate()[0]

	# Send the content of file to the recipient with
	# the given subject
	def mail(self, recipient, subject, file):
		cmd = self.sendmail
		cmd = re.sub("%subject"  , subject  , cmd)
		cmd = re.sub("%recipient", recipient, cmd) 
		cmd = re.sub("%file"     , file     , cmd)

		os.system(cmd)

