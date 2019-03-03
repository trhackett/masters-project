import random

def read_airports():
	f = open("airportcodes.txt", 'r')
	l = []
	for line in f:
		# up to the last character
		l.append(line[:-1])
	return l

def get_random_route(codes_list, num_codes):
	s = ""
	for i in range(num_codes):
		s += codes_list[random.randint(0,len(codes_list)-1)]
	return s

def write_routes(file, num_routes, start):
	for _ in range(num_routes):
		  # between 2 and 5 airports on each route
		rand_num = random.randint(2,5)
		  # write to output (input)
		final_file.write(start+get_random_route(airports, rand_num)+'\n')

airports = read_airports()

final_file = open("routes.txt",'w')

# LAX
final_file.write("LAX 5\n") # name num_routes
write_routes(final_file, 5, "LAX")
# CVG
final_file.write("CVG 8\n")
write_routes(final_file, 8, "CVG")
# ABQ
final_file.write("ABQ 12\n")
write_routes(final_file, 12, "ABQ")
