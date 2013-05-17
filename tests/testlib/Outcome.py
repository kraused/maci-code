
import os

# Outcome: Represents the results of a job
class Outcome:

	# Constructor. state can be either "FINISHED" or "ABORTED"
	def __init__(self, job, state):
		if not state in [ "FINISHED", "ABORTED" ]:
			raise Exception("state = '%s' invalid." % state)

		self.__name  = job.name
		self.__state = state
		self.__ok    = os.path.exists(job.workdir + "/OK")

		if self.__ok:
			self.__time = float(open(job.workdir + "/OK", "r").read())

	def __str__(self):
		if self.__ok:
			return "%s ... %s, OK, %f" % (self.__name.ljust(52), self.__state.ljust(8), self.__time)
		else:
			return "%s ... %s, FAIL"   % (self.__name.ljust(52), self.__state.ljust(8))


	def ok(self):
		return self.__ok

