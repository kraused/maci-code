#!/usr/bin/env ruby

# TODO Make the ppp a versatile util which can load different plugins to modify
#      code
# TODO Maybe one can do the preprocessing at runtime incorporating knowledge?
#      This would certainly be more complicated as we need to cope with imports

# ------------------------------------------------------------------------------
# Utility functions for output
# ------------------------------------------------------------------------------
# die: Kill the application and print an error message
def die(s)
	$stderr.puts " error: " + s
	Process.exit
end

# Count the number of occurences of a pattern in a string
def noccs(s, pattern)
	return s.scan(pattern).inject(0) { |sum,i| sum + 1 }
end

# Count the number of occurences of a pattern in an array
def nocca(a, pattern)
	return a.inject(0) { |sum,i| sum + (pattern.match(i) ? 1 : 0) }
end

# Indented print. The global variable $indentor must be set by the user
$indentor = 0
def iprint(arg)
	print($indentor + arg)
end

# Get a new variable
$var = 0
def get_var()
	$var = $var + 1
	return sprintf("var%03d", $var)
end

# Check if a variable is valid
def err_if_none(name, msg, indent = "")
	iprint("#{indent}if None == #{name}:\n")
	iprint("#{indent}\tprint >> sys.stderr, \"parser error: #{msg}.\"\n")
	iprint("#{indent}\tsys.exit(128)\n")
end

# Check if a condition is not fulfilled
def err_if_not(cond, msg, indent = "")
	iprint("#{indent}if not ( #{cond} ):\n")
	iprint("#{indent}\tprint >> sys.stderr, \"parser error: #{msg}.\"\n")
	iprint("#{indent}\tsys.exit(128)\n")
end

# ------------------------------------------------------------------------------
# Parse information
# ------------------------------------------------------------------------------
# keyworks
PARSE_START = "%parse"
PARSE_END   = "%esrap"
BLOCK_START = "%block"
BLOCK_END   = "%kcolb"

# VariableType: Defines the type of a variable
class VariableType
	OPTIONAL  =  0
	MANDATORY =  1

	STRING	  =  2
	INT	  =  4
	FLOAT	  =  8
	SOMETHING = 16

	@@map = [ [ /string[^\[\w]/    	      , STRING   ,  0 ],
	  	  [ /int[^\[\w]/       	      , INT      ,  0 ],
		  [ /float[^\[\w]/  	      , FLOAT    ,  0 ],
		  [ /int\[[0-9*]+\][^\[\w]/   , INT      , -1 ],
		  [ /float\[[0-9*]+\][^\[\w]/ , FLOAT    , -1 ],
		  [ /string\[[0-9*]+\][^\[\w]/, STRING   , -1 ],
		  [ /something[^\[\w]/        , SOMETHING,  0 ] ]
	
	# n is the size of the array. A value of (-1) indicates a variable size 
	# array.
	attr_accessor :type, :n

	# Reads the string and chops it
	def initialize(s)
		@type = 0
		@type = ( /mandatory/.match(s) ) ? MANDATORY : OPTIONAL

		match = 0
		@@map.each do |e|
			if e[0].match(s)
				@type |= e[1]
				match  = 1
			else
				next
			end

			if 0 == e[2]
				@n = 1
			else
				i  = s.index('[')
				k  = 0
				j  = s[i..s.length].index(']')

				if '*' == s[i+1..i+j-1]
					@n = -1
				else
					# Remove trailing zeros since Ruby
					# inteprets them as octal values
					k  = s[i+1..i+j-1].index(/[^0]/)
					@n = Integer(s[i+k+1..i+j-1])
				end
			end
		end

		if 0 == match
			die("unable to parse string \"#{s}\"")
		end
	end

	# Remove the string identifier of the type
	def chop(s)
		s.gsub!(/optional/ , "")
		s.gsub!(/mandatory/, "")

		@@map.each do |e|
			s.gsub!(e[0], "")
		end

		s.strip!()
	end

	# Return true if a variable is declared as being mandatory
	def is_mandatory()
		return (1 == @type & MANDATORY)
	end

	# Convert a read value
	def convert(val, name, indent = "")
		# Convert lambda
		conv = nil
		if    STRING == (@type - (@type & 1))
			conv = lambda { |v| "str(#{v})" }
		elsif INT    == (@type - (@type & 1))
			conv = lambda { |v| "int(#{v})" }
		elsif FLOAT     == (@type - (@type & 1))
			conv = lambda { |v| "float(#{v})" }
		elsif SOMETHING == (@type - (@type & 1))
			conv = lambda { |v| "#{v}.toxml()" }
		end		

		if @n == 1
			t = get_var()
			iprint("#{indent}#{t} = " + conv.call(val) + "\n")
			return t
		end

		array = get_var()
		# Need to handle newlines and tabs here
		iprint("#{indent}#{val} = re.sub('(\\t|\\n)',' ', #{val})\n")
		iprint("#{indent}#{val} = re.sub('[ ]+', ' ', #{val})\n")
		iprint("#{indent}#{array} = #{val}.split(' ')\n")

		if @n > 1
			err_if_not("len(#{array}) == #{@n}",
				"variable #{name} is expected to have length #{@n}",
				indent)
		end

		t = get_var()
		iprint("#{indent}#{t} = []\n")
		loop_var = get_var()
		iprint("#{indent}[ #{t}.append(" + conv.call(loop_var + ".strip()") + \
		       ") for #{loop_var} in #{array} ]\n")

		return t
	end

	# Returns true if we know something about the type
	def is_known_type()
		return !(SOMETHING == (@type - (@type & 1)))
	end
end

# Target: A target in which the parsed data is plugged in
# A target can either be a variable to be written or a callback
# function
class Target
	attr_accessor :name, :type

	VARIABLE = 1
	CALLBACK = 2

	VARIABLE_STRING = "->"
	CALLBACK_STRING = "=>"

	def initialize(arg)
		if 1 != noccs(arg,/#{Target::VARIABLE_STRING}/) + \
			noccs(arg,/#{Target::CALLBACK_STRING}/)
			die("invalid arg \"#{arg}\"")
		end

		@type = (/#{Target::CALLBACK_STRING}/.match(arg)) ? CALLBACK : VARIABLE
		@name = arg[/(#{Target::CALLBACK_STRING}|#{Target::VARIABLE_STRING}).+$/][2..-1].strip()
	end

	def chop(arg)
		arg.gsub!(/(#{Target::CALLBACK_STRING}|#{Target::VARIABLE_STRING}).+$/,"")
	end

	# Store read value in the target or call the callback function
	def store(val, indent = "")
		if    VARIABLE == @type:
			iprint("#{indent}#{@name} = #{val}\n")
		elsif CALLBACK == @type:
			iprint("#{indent}#{@name}(#{val})\n")
		end
	end
end

# parse_name: Parse the argument out of a the start/end of a tokenlist and chop the
#	      the list
def parse_arg_chop(tokens, id_start, id_end)
	if not /#{id_start}/.match(tokens[0]) or \
	       id_end != tokens[tokens.length-1]
		die("#{id_start} or #{id_end} not at the expected place")
	end

	name   = tokens[0].slice(/\([a-zA-Z0-9_\[\](\s*#{Target::VARIABLE_STRING}\s*)(\s*#{Target::CALLBACK_STRING}\s*)]*\)+/).gsub(/[\(|\)]/,"")

	tokens.slice!(0)
	tokens.slice!(tokens.length-1)

	return name
end

# Get a list of blocks from a list of tokens
def blocks_in_tokenlist(tokens)
	blks  = []
	level = 0
	start = 0

	# List of areas to remove from the tokenlist
	areas = []

	tokens.each_with_index do |t,i|
		if /#{BLOCK_START}/.match(t)
			level += 1
			if level == 1
				start = i
			end
		elsif BLOCK_END == t
			level -= 1
			if level == 0
				blks  += [ tokens[start..i] ]
				areas += [ [ start, i ] ]
			end
		end
	end

	areas.each { |a| (a[0]..a[1]).each { |k| tokens[k] = nil } }
	tokens.compact!

	return blks
end

# Block: A block
class Block
	attr_accessor :name, :attr, :blocks, :nodes
 
 	def initialize(tokens)
		parse_arg(parse_arg_chop(tokens, BLOCK_START, BLOCK_END))
		recurse(tokens)
 	end
 
	def parse_arg(arg)
		list = []
		arg.each(",") do |a|
			list += [ a.gsub(",","").strip() ]
		end

		if 0 == list.length
			die("block has unknown name")
		end

		@name = list.shift

		# Insert attributes
		@attr = []
		list.each do |k|
			@attr += [ Attribute.new(k) ]
		end
	end
		

	def recurse(tokens)
		@blocks = []
		@nodes  = []

		# Go through blocks
		blocks_in_tokenlist(tokens).each do |blk|
			@blocks += [ Block.new(blk) ]
		end

		tokens.each do |nds|
			@nodes += [ Node.new(nds.strip()) ]
		end

		# A block is by definition mandatory if any of its nodes or
		# attributes is mandatory
		@is_mandatory = false
		(@attr + @nodes).each { |v| @is_mandatory |= v.is_mandatory() }
	end

	def init_vars()
		# Recurse
		@blocks.each do |blk|
			blk.init_vars()
		end

		(@attr + @nodes).each do |x|
			x.init_var()
		end
	end

	def read_block(obj, indent)
		blockvar = get_var()
#		iprint("#{indent}#{blockvar} = #{obj}.getElementsByTagName(\'#{name}\')\n")
		iprint("#{indent}#{blockvar} = ppprt.FindChildElements(#{obj}, \'#{@name}\')\n")

#		if @is_mandatory:
			err_if_none(blockvar, 
				    "no element with tag name \\\"#{@name}\\\"",
				    indent)
			err_if_not("len(#{blockvar}) >= 1", 
				"\\\"#{@name}\\\" is a mandatory block",
				indent)
#		end
		# TODO Temporary fix to handle multiple blocks with the same
		#      name. Maybe this should be exported to the language?
		#      At least we should implement a mechanism to store the
		#      values from mutliple blocks with the same name in an
		#      array?!
#		err_if_not("len(#{blockvar}) <= 1", 
#			"expected exactly one element with tag name \\\"#{@name}\\\"",
#			indent)
#		iprint("#{indent}if len(#{blockvar}) == 1:\n")
#		iprint("#{indent}\t#{blockvar} = #{blockvar}[0]\n")
		loop_var = get_var()
		iprint("#{indent}for #{loop_var} in #{blockvar}:\n")
		iprint("#{indent}\t#{blockvar} = #{loop_var}\n")

		return blockvar
	end

	def read(obj, indent = "")
		blockvar = read_block(obj, indent)

		(@attr + @nodes).each do |nd|
			nd.read(blockvar, indent + "\t")
		end
		# recurse
		@blocks.each do |blk|
			blk.read(blockvar, indent + "\t")
		end
	end
end

# Variable: base class for nodes and attributes
class Variable
	attr_accessor :name, :type, :target

 	def initialize(arg)
		# Read the type and remove it from the string
		@type = VariableType.new(arg)
		@type.chop(arg)

		@target = Target.new(arg)
		@target.chop(arg)
		
		@name   = arg.strip()
 	end

	def init_var()
		if @target.type == Target::CALLBACK
			return
		end

		iprint("#{@target.name} = None\n")
	end

	def prepare_var(var, indent = "")
		# We need to handle the SOMETHING variables
		# differently since we parse the node (as a string)
		# rather than the data
		if @type.is_known_type()
			iprint("#{indent}#{var} = #{var}[0].childNodes\n")
			err_if_none(var, "found elment \\\"#{@name}\\\" without child nodes",
					indent)
			err_if_not("len(#{var}) == 1", 
				"don't know to handle element with tag name " \
				"\\\"#{@name}\\\" which has multiple child nodes",
				indent)
			iprint("#{indent}#{var} = #{var}[0].data\n")
			err_if_none(var, "data associated with \\\"#{@name}\\\" is None",
				indent)
			iprint("#{indent}#{var} = #{var}.strip()\n")
		else
			iprint("#{indent}#{var} = #{var}[0]\n")
		end
	end

	def is_mandatory()
		return @type.is_mandatory()
	end
end

# Node: a node 
class Node < Variable
	attr_accessor :multi

 	def initialize(arg)
		if '*' == arg[0].chr
			@multi = true
			arg = arg[1..-1]
		else
			@multi = false
		end
		if '(' != arg[0].chr or ')' != arg[-1].chr
			die("invalid argument \"#{arg}\"")
		end
		arg = arg[1..-2]

		super(arg)

		if @multi and @target.type == Target::VARIABLE
			die("you really don't want to have a multi node with VARIABLE target")
		end
 	end

	def read(obj, indent = "")
		t1 = get_var()
#		iprint("#{indent}#{t1} = #{obj}.getElementsByTagName(\'#{@name}\')\n")
		iprint("#{indent}#{t1} = ppprt.FindChildElements(#{obj}, \'#{@name}\')\n")
		if @type.is_mandatory()
			err_if_none(t1, "\\\"#{@name}\\\" is a mandatory string",
					indent)
			err_if_not("len(#{t1}) != 0",
					"\\\"#{@name}\\\" is a mandatory string",
					indent)
		end

		# Skip the whole section if the length 0 is zero. This can't
		# happend for mandatory arguments but it is easier to leave the
		# check for all cases
		iprint("#{indent}if 0 != len(#{t1}):\n")

		if not @multi
			err_if_not("len(#{t1}) <= 1", 
				"don't know to handle multiply elements with tag name \\\"#{@name}\\\"",
				indent + "\t")
			prepare_var(t1, indent + "\t")

			t2 = @type.convert(t1, @name, indent + "\t")
			@target.store(t2, indent + "\t")
		else
			loop_var = get_var()
			iprint("#{indent}\tfor #{loop_var} in #{t1}:\n")
			t2 = get_var()
			# Thats a bad track: In order to use the prepare_var
			# function we need to turn into an array
			iprint("#{indent}\t\t#{t2} = [ #{loop_var} ] \n")
			prepare_var(t2, indent + "\t\t")

			t2 = @type.convert(t2, @name, indent + "\t\t")
			@target.store(t2, indent + "\t\t")
		end
	end
end

# Attribute: an attribute
class Attribute < Variable
 	def initialize(arg)
		super(arg)
 	end

	def read(obj, indent = "")
		t1 = get_var()
		iprint("#{indent}#{t1} = #{obj}.getAttribute(\'#{@name}\')\n")
		if @type.is_mandatory()
			err_if_none(t1, "\\\"#{@name}\\\" is a mandatory attribute",
					indent)
			err_if_not("len(#{t1}) != 0",
					"\\\"#{@name}\\\" is a mandatory attribute",
					indent)
		end

		iprint("#{indent}if 0 != len(#{t1}):\n")

		t2 = @type.convert(t1, @name, indent + "\t")
		@target.store(t2, indent + "\t")
	end
end

 
# Document: The main document
# The document itself is a block which allows us to use the recursion mechanism
# of the block
class Document < Block
	def initialize(tokens)
		if 1 != nocca(tokens, /#{PARSE_START}/) or \
		   1 != nocca(tokens, /#{PARSE_END}/)
			die("too many \%parse or \%esrap")
		end

		# The %parse token has the filename as the argument
		# This is the major difference with the blocks and the reason
		# why don't call the super() function here
		@name = parse_arg_chop(tokens, PARSE_START, PARSE_END)

		@attr = []
		recurse(tokens)	
	end

	def readfile()
		dom = get_var()

                # Import the needed modules on our own so that the user does not need to
		# care which implementation is used under the hood
		# Since python modules are initialized only once it is okay to 
		# load the module multiple times
		iprint("import xml.dom.minidom\n")
		iprint("import re\n")
		iprint("import sys\n")
		iprint("import ppprt\n")


		# Open a file if the name contains FILE
		if    @name[0..3] == "FILE"
			@name = @name.gsub("FILE ","")
			iprint("#{dom} = ppprt.ParseFile(#{@name})\n")
			err_if_none(dom, "failed to open \\\"#{@name}\\\"")
		elsif @name[0..5] == "STRING"
			@name = @name.gsub("STRING ","")
			# Import the module on our own so that the user does not need to
			# care which implementation is used under 
			iprint("#{dom} = ppprt.ParseString(#{@name})\n")
			err_if_none(dom, "invalid string \\\"#{@name}\\\"")
		else
			iprint("#{dom} = #{@name}\n")
			err_if_none(dom, "invalid dom \\\"#{@name}\\\"")
		end

		@nodes.each do |nd|
			nd.read(dom)
		end
		@blocks.each do |blk|
			blk.read(dom)
		end
	end
end

# ------------------------------------------------------------------------------
# Parse input
# ------------------------------------------------------------------------------
# Strip the region
def strip_region(region)
	# Now we can strip the lines
	def mystrip(s)
		return s.gsub(/\s+/ ," ").gsub(/\s\(/,"(").strip()
	end

	return region.inject { |text, line|  mystrip(text) + mystrip(line) } 
end

# Break up region into token
def create_token_list(region)
	token = [ ]
	i     = 0
	while i < region.length
		c = region[i].chr
	
		if '%' == c
			k = 1
			if    region[i..i+PARSE_START.length] == "#{PARSE_START}("
				k = PARSE_START.length + region[i+PARSE_START.length..region.length].index(')')
			elsif region[i..i+BLOCK_START.length] == "#{BLOCK_START}("
				k = BLOCK_START.length + region[i+PARSE_START.length..region.length].index(')')
			elsif region[i..i+PARSE_END.length-1] == "#{PARSE_END}"
				k = PARSE_END.length-1
			elsif region[i..i+BLOCK_END.length-1] == "#{BLOCK_END}"
				k = PARSE_START.length-1
			end
	
			token += [ region[i..i+k] ]
			i     += k
			next
		elsif '*' == c or '(' == c
			if '*' == c and '(' != region[i+1].chr
				continue
			end
			token += [ region[i..i+region[i..region.length].index(')')] ]
			i     += region[i..region.length].index(')')
		end
	
		i += 1
	end

	return token
end

# ------------------------------------------------------------------------------
# Parse input file
# ------------------------------------------------------------------------------

# TODO Check input
f       = File.open(ARGV[0], "r")
$stdout = File.open(ARGV[1], "w")

region    = ""
in_region = false
f.each_line do |line|
	if '' != line.strip() and '#' == line.strip()[0].chr
		next
	end
	if /#{PARSE_START}/.match(line)
		in_region = true
	end
	if in_region
		region += line
	else
		puts line
	end
	if /#{PARSE_END}/.match(line)
		# First we need to find the correct indentation level in order 
		# not to confuse Python
		$indentor = region[0, region.index(PARSE_START)]

		# Start parsing
		doc = Document.new(create_token_list(strip_region(region)))

		# Initialize all veriables with None
		doc.init_vars()
		iprint("\n")

		# Parse the xml file
		doc.readfile()

		region = ""
		in_region = false
	end
end
f.close()
