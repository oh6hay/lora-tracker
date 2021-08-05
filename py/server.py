#!/usr/bin/env python3

import serial, sys
import tornado.ioloop
import tornado.web
import logging
import db, transforms

logging.basicConfig(level=logging.DEBUG)

class DataHandler(tornado.web.RequestHandler):
    def get(self):
        #self.write({'points': list(map(lambda x:x.__dict__(), db.get_all_positions()))})
        self.write({'points': list(map(transforms.position2geojsonobject,
                                       db.get_all_positions()))})

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.write(open('map.html','r').read())

def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/data/", DataHandler),
    ])

def main():
    app = make_app()
    if len(sys.argv) != 3:
        sys.stderr.write("usage: %s listen-port listen-host\n" % sys.argv[0])
        sys.stderr.write("If you run on anything else than inside a docker container,\n")
        sys.stderr.write("listen-host should be 127.0.0.1 unless you know what you are doing\n")
        sys.exit(0)
    app.listen(int(sys.argv[1]), sys.argv[2])
    tornado.ioloop.IOLoop.current().start()

if __name__ == "__main__":
    main()

    
