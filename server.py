import sys

from handlers import ThreadedHTTPServer, HTTPRequestHandler
from struct import pack

if __name__ == "__main__":
    EDU_WIFI = 0
    
    if EDU_WIFI:
        ip = "10.195.29.164"
    else:
        ip = "192.168.1.5"

    server = ThreadedHTTPServer((ip, 8000), HTTPRequestHandler)
    
    print("Starting server at", server.server_address[0] + ":" + str(server.server_address[1]))
    
    # CatID1 = 14155138199436
    # CatID2 = 14155138203326
    # CatID3 = 14155138203640
    
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        sys.exit(0)
