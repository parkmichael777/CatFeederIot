from socketserver import ThreadingMixIn
from http.server import BaseHTTPRequestHandler, HTTPServer

from struct import pack

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    pass

class HTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200, "OK")
        self.end_headers()
        self.flush_headers()

        self.wfile.write(pack('BBBBBBBB', 56, 1, 1, 1, 1, 1, 1, 1))
    
