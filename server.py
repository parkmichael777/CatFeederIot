import sys

from classes import ThreadedHTTPServer, HTTPRequestHandler

if __name__ == "__main__":
    server = ThreadedHTTPServer(("192.168.1.3", 8000), HTTPRequestHandler)
    
    print("Starting server at", server.server_address[0] + ":" + str(server.server_address[1]))
    
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        sys.exit(0)
