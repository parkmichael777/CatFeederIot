from socketserver import ThreadingMixIn
from http.server import BaseHTTPRequestHandler, HTTPServer
import threading
import shutil

from struct import pack, unpack

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    profile_version = int(0)
    profile_lock = threading.Lock()
    data_lock = threading.Lock()
    home_dir = "webapp"
    pass

class HTTPRequestHandler(BaseHTTPRequestHandler):
    close_connection = True

    def device_get_handler(self):
        #TODO: ADD LOCK
    
        device_version = int(self.headers["Cat-Profile-Version"])
        
        if device_version == self.server.profile_version:
            self.send_response(204, "No Content")
            self.end_headers()
            self.flush_headers()
            return

        self.send_response(200, "OK")
        self.send_header("Cat-Profile-Version", self.server.profile_version)
        self.end_headers()
        self.flush_headers()

        self.server.profile_lock.acquire()
        with open(self.path, 'rb') as f:
            profiles = f.read()
        self.server.profile_lock.release()
        
        self.wfile.write(profiles)

    def client_get_handler(self):
        if self.requestline == "":
            self.send_response(400, "Bad Request")
            self.end_headers()
            self.flush_headers()
            return

        with open(self.server.home_dir + self.path, 'rb') as f:
            self.send_response(200, "OK")
            self.end_headers()
            self.flush_headers()
            
            for line in f:
                self.wfile.write(line)
        
    def device_post_handler(self):
        self.send_response(200, "OK")
        self.end_headers()
        self.flush_headers()

        cat_idx, amt_eat, timestamp = unpack("!ifQ", self.rfile.read())
        print("Cat:", cat_idx);
        print("Amt Eaten:", amt_eat)
        print("Time:", timestamp)

        # TODO: save to file with lock
        # TODO: update profile version.

    def client_post_handler(self):
        if self.headers["Cat-Profile-Index"] == None:
            self.send_response(400, "Bad Request")
            self.end_headers()
            self.flush_headers()
            return
        
        self.send_response(200, "OK")
        self.end_headers()
        self.flush_headers()
            
        cat_profile = int(self.headers["Cat-Profile-Index"])
        message = self.rfile.read()
        
        self.server.profile_lock.acquire()
        with open("CatProfiles", 'wb') as f:
            f.seek(58 * cat_profile)        # 58 bytes == len(cat profile)
            f.write(message)
            self.server.profile_version += 1
        self.server.profile_lock.release()

        #TODO: check inUse to add or delete data file.
        
        in_use = unpack("!B", message)
        
        if in_use == 0:
            shutil.rmtree(self.server.home_dir + "/" + str(cat_profile))
        
#        in_use, max_rate, portion_grams, num_portions, p0, p1, p2, p3, p4, cat_id = unpack("!BffBQQQQQQ", message)
#
#        print("inUse:", in_use)
#        print("maxRate:", max_rate)
#        print("portionGrams:", portion_grams)
#        print("numPortions:", num_portions)
#        print("Time 0:", p0)
#        print("Time 1:", p1)
#        print("Time 2:", p2)
#        print("Time 3:", p3)
#        print("Time 4:", p4)
#        print("CatID:", cat_id)

    def do_GET(self):
        if self.headers["Cat-Request-Type"] == "device":
            self.device_get_handler()
        else:
            self.client_get_handler()
        
    def do_POST(self):
        # Device posts are sending collected data.
        if self.headers["Cat-Request-Type"] == "device":
            self.device_post_handler()
        # Client posts are updates to profiles.
        else:
            self.client_post_handler()
    
