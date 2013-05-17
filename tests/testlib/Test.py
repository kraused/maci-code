
import json
import re

from Job import Job

# Test: A class representing a regression test.
class Test:

	# Create a new test from the JSON representation. The
	# argument pwd equals the directory in which the tests.json
	# file is located.
	def __init__(self, pwd, str):
		self.__dict__["table"] = {	# The name of the test
						"name" : "anonymous",
						# The description of the test.
						"descr" : "undescribed",
						# The number of processing elements
						# to use for testing. For each member
						# of the array the test will be executed
						# once
						"npes" : [],
						# Maximal walltime for this test. By
						# default tests can be maximal 15min.
						"walltime" : "00:15:00",
						# The executable arguments.
						"arguments" : "",
						# The produced output which should be
						# checked
						"output" : [],
						# The prerequisite for this test. The
						# test is ignored if the executable does
						# not fulfill these requirements (see the
						# "conf" member of the Env)
						"prereq" : [] }

		for k, v in json.read(str).items():
			# Just a check
			if "type" == k:
				assert "test" == v
			self.table[k] = v

		# Process the argument string
		self.arguments = re.sub("%pwd", pwd, self.arguments)

	def __getattr__(self, k):
		return self.table[k]

	def __setattr__(self, k, v):
		self.table[k] = v

        def __repr__(self):
                return repr(self.table)

        def __str__(self):
                return str(self.table)

	# Create jobs from the test. 
	def jobify(self, env):
		# If the environment does not fulfill our
		# requirements we must skip the test
		for p in self.prereq:
			if 0 == env.config.count(p):
				return None
	
		jobs = []
		[ jobs.append(Job(self.name + ".%d" % p, p, self.walltime, env.exe, self.arguments, env.output + "/" + self.name + ".%d" % p)) for p in self.npes ]

		return jobs

