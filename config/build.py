#!/usr/bin/env python

import sys
import os
import optparse
import xml.dom.minidom
import shutil
import subprocess
import select
import tempfile

# Read a string
# @param el	the element
# @param name	element name
def xml_read_string(el, name):
	return str(el.getElementsByTagName(name)[0].childNodes[0].data.strip())

# Let a child process execute a command
def child_exec(child, cmd):
	child.stdin.write(cmd + '\n')

# A dependency
class Dependency:
	
	# Create a new dependency from an xml node
	# @param p	the xml file node
	def __init__(self, p):
		self.name = xml_read_string(p, 'name')
		if 0 < len(p.getElementsByTagName('lib')):
			self.lib = '"' + xml_read_string(p, 'lib') + '"'
		else:
			self.lib = None
		if 0 < len(p.getElementsByTagName('include')):
			self.include = '"' + xml_read_string(p, 'include') + '"'
		else:
			self.include = None
		if 0 < len(p.getElementsByTagName('names')):
			self.names = '"' + xml_read_string(p, 'names') + '"'
		else:
			self.names = None

# A software package
class Package:

	# Create a new package from an xml node
	# @param p	the xml file node
	def __init__(self, p):
		self.name = xml_read_string(p, 'name')
		self.dir  = xml_read_string(p, 'dir' )

		# resolve environment variables
		while -1 != self.dir.find('$'):
			var 	 = self.dir[self.dir.find('$'):len(self.dir)].split('/')[0]
			val 	 = os.environ[var[1:len(var)]]
			self.dir = self.dir.replace(var,val)

		if not os.access(self.dir, os.X_OK):
			sys.exit(' error: cannot access directory \'%s\'' % self.dir)

		# Read the type
		if 0 < len(p.getElementsByTagName('type')):
			self.type = xml_read_string(p, 'type').lower()
		else:
			self.type = 'undefined'

		# read options
		self.opts = {}
		if None != p.getElementsByTagName('option'):
			for nd in p.getElementsByTagName('option'):
				self.opts[xml_read_string(nd,'key')] = xml_read_string(nd,'val')

		# read dependencies
		self.deps = []
		if None != p.getElementsByTagName('dependency'):
			for nd in p.getElementsByTagName('dependency'):
				self.deps.append(Dependency(nd))

# Configuration: A class representation of the configuration of a software bundle
class Configuration:
	
	# Create a new configuration from an xml file
	# @param arg	the input xml file
	def __init__(self, arg):
		self.__inputfile = arg

		config = xml.dom.minidom.parse(arg).getElementsByTagName('config')
		if len(config) < 1:
			sys.exit(' error: no element with tag name \'config\' found')

		self.__name = xml_read_string(config[0], 'name')
		self.__type = xml_read_string(config[0], 'type').lower()

		if 'release' != self.__type and 'debug' != self.__type:
			sys.exit(' error: unsupported config type \'%s\'' % self.__type)

		# The path where we build all packages
		self.__path = os.getcwd() + '/' + self.__name + '/' + self.__type

		# read modules
		self.__mods = []
		if 0 < len(config[0].getElementsByTagName('modules')):
			self.__mods = xml_read_string(config[0], 'modules').split(' ')

		# read options
		self.__opts = {}
		if 0 < len(config[0].getElementsByTagName('option')):
			for nd in config[0].getElementsByTagName('option'):
				self.__opts[xml_read_string(nd,'key')] = xml_read_string(nd,'val')

		# read packages
		self.__packages = []
		for p in config[0].getElementsByTagName('packages')[0].getElementsByTagName('package'):
			P = Package(p)

			if 'undefined' == P.type:
				P.type = self.__type

			for k,v in self.__opts.iteritems():
				if not k in P.opts:
					P.opts[k] = v

			self.__packages.append(P)
		
		# Handle dependencies
		for p in self.__packages:
			for d in p.deps:
				if None == d.lib and None == d.include:
					for q in self.__packages:
						if q.name == d.name:
							d.lib     = self.__path + '/' + q.name + '/lib'
							d.include = self.__path + '/' + q.name + '/include'

	# Create folder
	# The function returns the path to the directory
	def create_folder(self):
		path = ''
		for dir in self.__path.split('/'):
			path = path + dir + '/'
			try:
				os.mkdir(path)
			except:
				assert os.access(path, os.X_OK)

	# Copy the input file into the target directory
	def copy_input_file(self):
		shutil.copy(self.__inputfile, self.__path)

	# Create a shell with the necessary modules loaded
	# @returns	the shell subprocess
	def open_shell(self):
		shell = subprocess.Popen(['/bin/bash','-l'], stdin = subprocess.PIPE)

		# module clear and module purge didn't really work out. Hence
		# I created my own "module purge" command which unloads the modules
		# in the reverse order of the loading
		child_exec(shell, 'for i in `module list 2>&1 | grep -v "Currently Loaded Modulefiles" | xargs perl -e \'@k = (); for($i = 0; $i < @ARGV; ++$i) { $s = $ARGV[$i]; if($s =~ /[0-9]+\)/) { $s =~ s/\)//g ; $k[int($s)-1] = $ARGV[$i+1]; }}; for($i = @k-1; $i >= 0; --$i) { print "$k[$i] "; }\'`; do module unload $i ; done')
		
		# Check
		child_exec(shell, 'if ! [[ "No Modulefiles Currently Loaded." == `module list 2>&1` ]] ; then echo "Should not happen!" ; fi')

		# Load the necessary modules		
		for module in self.__mods:
			child_exec(shell, 'module load %s' % module)

		child_exec(shell, 'module list')

		return shell

	# Exit the shell
	# @param	shell	the shell to exit
	def exit_shell(self, shell):
		child_exec(shell, 'exit $?')
		shell.wait()
		if 0 != shell.returncode:
			sys.exit(shell.returncode)

	# Configure a package
	# @param	p	the package to configure
	def config_package(self, p):
		try:
			os.mkdir(p.name)
		except:
			assert os.access(p.name, os.X_OK)

		os.chdir(self.__path + '/' + p.name)
		shell = self.open_shell()

		# Create the cmake command
		cmd = 'cmake '

		# ... add options
		for k,v in p.opts.iteritems():
			cmd = cmd + '-D%s=%s ' % (k,v)

		# ... add dependencies
		for d in p.deps:
			cmd = cmd + '-DTPL_ENABLE_%s:BOOL=ON ' % d.name
			if None != d.include:
				cmd = cmd + '-D%s_INCLUDE_DIRS=%s ' % (d.name, d.include)
			if None != d.lib:
				cmd = cmd + '-D%s_LIBRARY_DIRS=%s ' % (d.name, d.lib)
			if None != d.names:
				cmd = cmd + '-D%s_LIBRARY_NAMES=%s ' % (d.name, d.names)

		# ... the type
		if 'release' == p.type:
			cmd = cmd + '-DCMAKE_BUILD_TYPE=Release '
		if 'debug'   == p.type:
			cmd = cmd + '-DCMAKE_BUILD_TYPE=Debug '

		# ... the prefix
		cmd = cmd + '-DCMAKE_INSTALL_PREFIX=$PWD ' + p.dir

		print ' + %s' % cmd
		child_exec(shell, cmd)

		self.exit_shell(shell)
		os.chdir(self.__path)

	# Make a package
	# @param	p	the package to configure
	# @param	nthread	number of threads to use for configuring
	def make_package(self, p, nthreads):	
		try:
			os.mkdir(p.name)
		except:
			assert os.access(p.name, os.X_OK)

		os.chdir(self.__path + '/' + p.name)
		shell = self.open_shell()

		child_exec(shell, 'make install -j %d' % nthreads)

		self.exit_shell(shell)
		os.chdir(self.__path)

	# Configure everyting
	# @param jobs	number of allowed jobs
	def configure(self, jobs):
		path = self.create_folder()
		self.copy_input_file()	
	
		# Change the directory to the target directory
		os.chdir(self.__path)

		for p in self.__packages:
			self.config_package(p)
			self.make_package  (p, jobs)
			

if __name__ == "__main__":
        parser = optparse.OptionParser("usage: %prog file")
        parser.add_option("-j", "--jobs",
		          action  = "store",
		          type    = "int",
		          dest    = "jobs",
			  default = 1,
		          help    = "number of jobs allowed")
        (opts,args) = parser.parse_args()

        if 1 > len(args):
		parser.error("undefined input file")

	Configuration(args[0]).configure(opts.jobs)

