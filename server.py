import sys

from handlers import ThreadedHTTPServer, HTTPRequestHandler
from struct import pack

def time(hour, min, pm):
    if not pm and hour == 12:
        hour = 0
    elif pm and hour != 12:
        hour += 12
    
    t = hour * 3600000 + min * 60000
    print(t)
    return t

if __name__ == "__main__":
    server = ThreadedHTTPServer(("192.168.1.3", 8000), HTTPRequestHandler)
    
    print("Starting server at", server.server_address[0] + ":" + str(server.server_address[1]))
    
    hour = 2
    min = 20
    
    h3 = 6
    m3 = 2
    
    # Create test files
    f = open("CatProfiles", "wb")
    p = pack('!BfBBLLLLLQBfBBLLLLLQBfBBLLLLLQ',
        1, 1.75, 1, 3, time(hour, min, 1), time(hour, min + 2, 1), time(hour, min + 4, 1), 1, 1, 0,# TODO: Add catIDs.
        0, 2.75, 1, 3, time(hour, min, 1), time(hour, min + 2, 1), time(hour, min + 4, 1), 0, 0, 0,
        0, 3.75, 1, 3, time(h3, m3, 1), time(h3, m3 + 2, 1), time(h3, m3 + 4, 1), 0, 0, 0)
    
    f.write(p)
    
    f.close()
    
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        sys.exit(0)
