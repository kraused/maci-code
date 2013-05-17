
# Report the outcomes of the performed tests
def report(env, host, outcomes, filename):
	fo = open(filename, "w")

	fo.write(" ===== TEST RESULTS =====\n")
	fo.write(" HOST   : %s\n" % host.host)
	fo.write(" INPUT  : %s\n" % env.input)
	fo.write(" OUTPUT : %s\n" % env.output)
	fo.write(" EXE    : %s\n" % env.exe)
	fo.write(" CONFIG : %s\n" % env.config)
	fo.write("\n")
	
	fo.write(" OUTCOMES:\n")
	for o in outcomes:
		fo.write(" \t%s\n" % o)
	fo.write("\n")

	ntests = len(outcomes)
	nsucc  = 0
	for o in outcomes:
		if o.ok():
			nsucc += 1
	nfail  = ntests - nsucc

	fo.write(" SUMMARY:\n")
	fo.write(" # TESTS: %d\n" % ntests)
	fo.write(" SUCCESS: %d (%3.2f %%)\n" % (nsucc, 100*float(nsucc)/float(ntests)))
	fo.write(" FAILURE: %d (%3.2f %%)\n" % (nfail, 100*float(nfail)/float(ntests)))

	fo.close()

	# Send a mail to the user to report the
	# test outcomes
	host.mail(env.responsible, env.name + " @ " + env.now, filename)

