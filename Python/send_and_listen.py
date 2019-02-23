import random
import socket
import struct
import time

HEARTBEAT_FMT = "hh"


def loop(port):
    sock = socket.socket(socket.AF_INET,
                         socket.SOCK_DGRAM)  # UDP

    sock.bind(('', port))

    target_ip = '10.0.0.100'
    target_port = 8888

    count = 0

    while True:
        random_val = random.randint(0, 100)

        print("I sent count " + str(count) + " and value " + str(random_val))
        data = struct.pack(HEARTBEAT_FMT, count, random_val)
        sock.sendto(data, (target_ip, target_port))

        time.sleep(1)

        data, address = sock.recvfrom(1024)
        count, random_val = struct.unpack(HEARTBEAT_FMT, data)
        print("I received count " + str(count) + " and value " + str(random_val))

        count += 1


loop(8000)
