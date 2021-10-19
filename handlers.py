from socketserver import ThreadingMixIn
from http.server import BaseHTTPRequestHandler, HTTPServer

from struct import pack, unpack

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    pass

class HTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200, "OK")
        self.end_headers()
        self.flush_headers()

        f = open("CatProfiles", 'rb')
        profs = f.read(33)

        f.close()
        
#        mr0, gp0, np0, t00, t01, t02, t03, t04, mr1, gp1, np1, t10, t11, t12, t13, t14, mr2, gp2, np2, t20, t21, t22, t23, t24 = unpack("!fBBBBBBBfBBBBBBBfBBBBBBB", profs)
#        print(mr1)
        
        self.wfile.write(profs)
