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
    EDU_WIFI = 1
    
    if EDU_WIFI:
        ip = "10.195.29.164"
    else:
        ip = "192.168.1.3"

    server = ThreadedHTTPServer((ip, 8000), HTTPRequestHandler)
    
    print("Starting server at", server.server_address[0] + ":" + str(server.server_address[1]))
    
    cat_id_1 = 14155138199436
    cat_id_2 = 14155138203326
    cat_id_3 = 14155138203640
    
    hour = 9
    min = 37
    pm = 1
    
    h3 = 6
    m3 = 2
    
    # Create test files
    f = open("CatProfiles", "wb")
    pack_format = "BffBQQQQQQ"
    p = pack("!" + pack_format + pack_format + pack_format,
        1, 1.0, 5.0, 1, time(hour, min, pm), time(hour, min + 2, pm), time(hour, min + 4, pm), 1, 1, cat_id_1,
        0, 2.75, 1.0, 3, time(hour, min, pm), time(hour, min + 2, pm), time(hour, min + 4, pm), 0, 0, cat_id_2,
        0, 3.75, 1.0, 3, time(h3, m3, pm), time(h3, m3 + 2, pm), time(h3, m3 + 4, pm), 0, 0, cat_id_3)
    
    f.write(p)
    
    f.close()
    
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        sys.exit(0)
