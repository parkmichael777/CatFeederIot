from socketserver import ThreadingMixIn
from http.server import BaseHTTPRequestHandler, HTTPServer

from struct import pack, unpack
import numpy as np

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    server_version = np.uint(1)
    pass

class HTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        device_version = np.uint(self.headers["Cat-Profile-Version"])
        
#        print(device_version)
        
        if device_version == self.server.server_version:
            self.send_response(204, "No Content")
            self.end_headers()
            self.flush_headers()
            return
    
        self.send_response(200, "OK")
        self.send_header("Cat-Profile-Version", self.server.server_version)
        self.end_headers()
        self.flush_headers()

        f = open(self.path, 'rb')
        profs = f.read(78)

        f.close()
        
#        mr0, gp0, np0, t00, t01, t02, t03, t04, mr1, gp1, np1, t10, t11, t12, t13, t14, mr2, gp2, np2, t20, t21, t22, t23, t24 = unpack("!fBBBBBBBfBBBBBBBfBBBBBBB", profs)
#        print(mr1)
        
        self.wfile.write(profs)
