#!/usr/bin/env python3

import socket
import os
import random
import time



def main():

	if os.path.exists("./incr-num-serv"):
		os.remove("./incr-num-serv")
	print("file check done")


	active_connections_timing = {}

	server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
	server.bind("./incr-num-serv")
	print("bound, listening...")
	server.listen(10)
	while True:
		conn, addr = server.accept()
		print("\naccepted connection...")

		id_req = conn.recv(3)
		id_req = id_req.decode()
		print("got data: " + str(id_req))

		if(id_req not in active_connections_timing):
			# first value is the time this request came in
			# second value is the time this requester should wait
			active_connections_timing[id_req] = [time.time(), random.randint(0, 5)]
			wait_time = 0
		else:
			now = time.time()
			wait_time = now - active_connections_timing[id_req][0]


		print(active_connections_timing)
		print("wait_time: " + str(wait_time))
		if(wait_time > active_connections_timing[id_req][1]):
			num = random.randint(1, 3)
			del active_connections_timing[id_req]

		else:
			num = 0

		print("sending back: " + str(num))
		num_bytes = num.to_bytes(4, byteorder="little")
		conn.sendall(num_bytes);

		conn.close()

main()
