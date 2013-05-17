#!/usr/bin/env ruby

def stringify0(i)
	return sprintf("%02d", i)
end

def stringify1(i)
	return sprintf("%2d", i)
end

def create_datatype(n)
	puts "/** Create a derived datatype using MPI_Type_create_struct"
	puts " *  @param[out]	type\tthe derived datatype to create"
	puts " *  @param[in]	Struct\tname of the corresponding structure"
	(1..n).each do |i|
		si = stringify0(i)

		puts " *  @param[in]	v#{si}\tname of the struct member"
		puts " *  @param[in]	type#{si}\tMPI datatype corresponding to the member"
		puts " *  @param[in]	n#{si}\tlength of array (1 if no array)"
	end
	puts " */"
	print "#define create_struct#{n}(type, Struct,"
	(1..n).each do |i|
		si = stringify0(i)

		print " v#{si}, type#{si}, n#{si}" + \
				((i == n) ? ")\t\t\t\\\n" : ",\t\t\t\\\n\t\t\t\t")
	end

	sn = stringify0(n)

	ex    = "_mangle#{sn}_ex";
	blens = "_mangle#{sn}_blens";
	displ = "_mangle#{sn}_displ";
	addr  = "_mangle#{sn}_addr";
	types = "_mangle#{sn}_types";

	sn = stringify1(n)

	puts "\tdo {\t\t\t\t\t\t\t\t\\"
	puts "\t\tStruct       #{ex};\t\t\t\t\\"
	puts "\t\tint          #{blens}[#{sn}];\t\t\t\\"
	puts "\t\tMPI_Aint     #{displ}[#{sn}],\t\t\t\\\n\t\t\t\ \ \ \ \ #{addr}[#{sn}];\t\t\t\\"
	puts "\t\tMPI_Datatype #{types}[#{sn}];\t\t\t\\"
	puts "\t\t\t\t\t\t\t\t\t\\"
	(0..n-1).each do |i|
		si = stringify0(i+1)
		sj = stringify1(i)
		puts "\t\tMPI_Get_address(&#{ex}.v#{si},\t\t\t\\\n\t\t\t\t&#{addr}[#{sj}]);\t\t\t\\"
	end
	puts "\t\t\t\t\t\t\t\t\t\\"
	(0..n-1).each do |i|
		si = stringify0(i+1)
		sj = stringify1(i)

		if 0 == i
			puts "\t\t#{displ}[#{sj}] = 0;\t\t\t\t\\"
		else
			puts "\t\t#{displ}[#{sj}] = #{addr}[#{sj}]\t\t\\\n\t\t\t\t\t-#{addr}[0];\t\t\\"
		end
	end
	puts "\t\t\t\t\t\t\t\t\t\\"
	(0..n-1).each do |i|
		si = stringify0(i+1)
		sj = stringify1(i)
		puts "\t\t#{types}[#{sj}] = type#{si};\t\t\t\t\\"
	end
	puts "\t\t\t\t\t\t\t\t\t\\"
	(0..n-1).each do |i|
		si = stringify0(i+1)
		sj = stringify1(i)
		puts "\t\t#{blens}[#{sj}] = n#{si};\t\t\t\t\\"
	end
	puts "\t\t\t\t\t\t\t\t\t\\"
	puts "\t\tMPI_Type_create_struct(#{n}, #{blens},\t\t\\\n\t\t\t#{displ}, #{types}, &(type));\t\\"
	puts "\t\t\t\t\t\t\t\t\t\\"
	puts "\t} while(0)"
end

puts ""
puts "#ifndef CODE_MPI_CREATE_DATATYPE_HH_INCLUDED"
puts "#define CODE_MPI_CREATE_DATATYPE_HH_INCLUDED 1"
puts ""
puts "/* automatically created by ${__FILE__} */"
puts ""

(3..4).each do |i|
	create_datatype(i)
	puts ""
end

puts "#endif"
puts ""
