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
    EDU_WIFI = 0
    
    if EDU_WIFI:
        ip = "10.195.29.164"
    else:
        ip = "192.168.1.16"

    server = ThreadedHTTPServer((ip, 8000), HTTPRequestHandler)
    
    print("Starting server at", server.server_address[0] + ":" + str(server.server_address[1]))
    
    cat_id_1 = 14155138199436
    cat_id_2 = 14155138203326
    cat_id_3 = 14155138203640
    
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        sys.exit(0)
