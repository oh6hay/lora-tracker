#!/usr/bin/env python3

import logging, struct
logging.basicConfig(level=logging.DEBUG)


# enums

LOCATION_REQUEST = 1
LOCATION_RESPONSE = 2
LOCATION_NOTIFY = 3
LOCATION_STARTTRACKING_REQUEST = 4
LOCATION_STARTTRACKING_RESPONSE = 5
LOCATION_ENDTRACKING_REQUEST = 6
LOCATION_ENDTRACKING_RESPONSE = 7
LOCATION_SETINTERVAL_REQUEST = 8
LOCATION_SETINTERVAL_RESPONSE = 9
MESSAGE = 10

class lposition:
    def __init__(self):
        self.rssi = 0
        self.padding = 0
        self.padding2 = 0
        
    def parse(self, bs):
        fields = struct.unpack('<HHHHHHHHIIddddd', bs)
        (self.hour,
         self.minute,
         self.second,
         self.satellites,
         self.year,
         self.month,
         self.day,
         self.padding,
         self.agemillis,
         self.padding2,
         self.lat,
         self.lng,
         self.kmph,
         self.course,
         self.meters) = fields
        if self.agemillis == 4294967295:
            self.agemillis = -1

    def __repr__(self):
        r = ""
        r += "hour: %d\n"%self.hour
        r += "minute: %d\n"%self.minute
        r += "second: %d\n"%self.second
        r += "satellites: %d\n"%self.satellites
        r += "year: %d\n"%self.year
        r += "month: %d\n"%self.month
        r += "day: %d\n"%self.day
        r += "padding: %d\n"%self.padding
        r += "age(milliseconds): %d\n"%self.agemillis
        r += "padding2: %d\n"%self.padding2
        r += "latitude: %.4f\n"%self.lat
        r += "longitude: %.4f\n"%self.lng
        r += "kmph: %d\n"%self.kmph
        r += "course: %d\n"%self.course
        r += "altitude(meters): %d\n"%self.meters
        return r

    def __dict__(self):
        d = {'hour': self.hour,
             'minute': self.minute,
             'second': self.second,
             'satellites': self.satellites,
             'year': self.year,
             'month': self.month,
             'day': self.day,
             'age': self.agemillis,
             'lat': self.lat,
             'lng': self.lng,
             'kmph': self.kmph,
             'course': self.course,
             'altitude': self.meters}
        return d

class lpkt:
    def parse_header(self, bs):
        fields = struct.unpack('<BBBBBBBbI', bs[:12])
        (self.flags,
         self.message_type,
         self.srcaddr,
         self.dstaddr,
         self.fragmentno,
         self.fragments,
         self.payload_size,
         self.rssi,
         self.seqno) = fields

    def __repr__(self):
        r = "flags: %02x\n" % self.flags
        r += "message_type: %02x\n" % self.message_type
        r += "srcaddr: %02x\n" % self.srcaddr
        r += "dstaddr: %02x\n" % self.dstaddr
        r += "fragmentno: %02x\n" % self.fragmentno
        r += "fragments: %02x\n" % self.fragments
        r += "payload_size: %02x\n" % self.payload_size
        r += "RSSI: %d\n" % self.rssi
        r += "seqno: %02x\n" % self.seqno
        return r
        
        
        
