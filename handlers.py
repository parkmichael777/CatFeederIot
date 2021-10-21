from socketserver import ThreadingMixIn
from http.server import BaseHTTPRequestHandler, HTTPServer

from struct import pack, unpack
import numpy as np

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    profile_version = np.uint(1)
    num_cats = 3
    pass

class HTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        device_version = np.uint(self.headers["Cat-Profile-Version"])
        
#        print(device_version)
        
        if device_version == self.server.profile_version:
            self.send_response(204, "No Content")
            self.end_headers()
            self.flush_headers()
            return
    
        self.send_response(200, "OK")
        self.send_header("Cat-Profile-Version", self.server.profile_version)
        self.send_header("Cat-Quantity", self.server.num_cats)
        self.end_headers()
        self.flush_headers()

        f = open(self.path, 'rb')
        profs = f.read()
        f.close()
        
        self.wfile.write(profs)
