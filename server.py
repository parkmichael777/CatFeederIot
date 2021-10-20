import sys

from handlers import ThreadedHTTPServer, HTTPRequestHandler
from struct import pack

if __name__ == "__main__":
    server = ThreadedHTTPServer(("192.168.1.3", 8000), HTTPRequestHandler)
    
    print("Starting server at", server.server_address[0] + ":" + str(server.server_address[1]))
    
    # Create test files
    f = open("CatProfiles", "wb")
    p = pack('!fBBLLLLLfBBLLLLLfBBLLLLL', 1.75, 1, 1, 1, 1, 1, 1, 1,
                                          2.75, 2, 2, 2, 2, 2, 2, 2,
                                          3.75, 3, 3, 3, 3, 3, 3, 3)
    
    f.write(p)
    
    f.close()
    
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        sys.exit(0)
