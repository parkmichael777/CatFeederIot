from socketserver import ThreadingMixIn
from http.server import BaseHTTPRequestHandler, HTTPServer
import threading
import shutil

from struct import pack, unpack

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    profile_version = int(0)  # Must acquire profile_lock to use.
    profile_lock = threading.Lock()
    data_lock = threading.Lock()
    webapp_dir = "webapp"
    data_dir = webapp_dir + "/data"
    pass

class HTTPRequestHandler(BaseHTTPRequestHandler):
    close_connection = True

    def device_get_handler(self):
        device_version = int(self.headers["Cat-Profile-Version"])
        
        self.server.profile_lock.acquire()
        if device_version == self.server.profile_version:
            self.server.profile_lock.release()

            self.send_response(204, "No Content")
            self.end_headers()
            self.flush_headers()
            return

        self.send_response(200, "OK")
        self.send_header("Cat-Profile-Version", self.server.profile_version)
        self.end_headers()
        self.flush_headers()

        with open(self.path, 'rb') as f:
            for line in f:
                self.wfile.write(line)
        self.server.profile_lock.release()

    def client_get_handler(self):
        if self.requestline == "":
            self.send_response(400, "Bad Request")
            self.end_headers()
            self.flush_headers()
            return
            
        self.send_response(200, "OK")
        self.end_headers()
        self.flush_headers()
        
        data_request = 0;
        if "data/" in self.requestline:
            self.server.data_lock.acquire()
            with open(self.server.webapp_dir + self.path, 'rb') as f:
                for line in f:
                    self.wfile.write(line)
            self.server.data_lock.release()
        else:
            with open(self.server.webapp_dir + self.path, 'rb') as f:
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
        
        in_use, max_rate, portion_grams, num_portions, p0, p1, p2, p3, p4, cat_id = unpack("!BffBQQQQQQ", message)
        
        # If a profile was deleted, delete the profile's data.
        if in_use == 0:
            shutil.rmtree(self.server.data_dir + "/" + str(cat_profile))

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
    
