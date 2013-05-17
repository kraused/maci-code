#!/usr/bin/env python

import os
import sys
import testlib
import time

assert 2 <= len(sys.argv)

# Load the env
E = testlib.Env(sys.argv[1])

if False == os.path.exists(E.output):
	os.makedirs(E.output)

os.chdir(E.output)

# Read the host configuration
H = testlib.Host(E.host)

# Create walker and read the tests
T = testlib.Walker(E.input).walk()

# Create the jobs
jobs  = []
for t in T:

	if 3 <= len(sys.argv):
		if not t.name in sys.argv[2:]:
			continue

	j = t.jobify(E)
	if None != j:
		jobs.extend(j)

if 0 == len(jobs):
	print " NO JOBS. EXITING ..."
	sys.exit(0)

# Fire them ..
for j in jobs:
	j.id = H.submit(E, j)
#	print " JOB %s SUBMITTED: %s" % (j.name, j.id.strip())

# The list of outcomes
outcomes = []

# SLEEP_TIME defines how much time the script should sleep
# between each check of the job statuses
SLEEP_TIME = 15
NITERS     = int(E.max_walltime*60.0/SLEEP_TIME)

# Go in the wait cycle
for i in range(0, NITERS):
	print " ===== Wait iteration %2d =====" % (i+1)
	time.sleep(SLEEP_TIME)

	b = True
	while b:
		b = False
		for j in jobs:
			if H.finished(j):
#				print " JOB %s FINISHED." % j.id.strip()
				jobs.remove(j)
				outcomes.append(testlib.Outcome(j, "FINISHED"))

				b = True
				break

#	print "JOBS LEFT: %s" % jobs
	print " ============================="

	# Done all jobs finished
	if 0 == len(jobs):
		break

# Remove any left over jobs
for j in jobs:
	H.delete(j)
	outcomes.append(testlib.Outcome(j, "ABORTED"))


# Write a report
testlib.report(E, H, outcomes, "report.txt")

print " DONE."
sys.exit(0)

