#!/usr/bin/env python3
import serial, sys
from messages import *
import db

def read_until_packet(sp):
    q = []
    while True:
        q.append(int(sp.read(1)[0]))
        if len(q) < 4:
            continue
        while len(q) > 4:
            del q[0]
        logging.debug(' '.join(map(lambda x:'%02x'%x, q)))
        if q[0] == 0x12 and q[1] == 0x34 and q[2] == 0x56 and q[3] == 0x78: #delim found
            logging.debug('delimiter found, reading more')
            header_bytes = sp.read(16)
            logging.debug('got %d bytes'%len(header_bytes))
            logging.debug(' '.join(map(lambda x:'%02x'%x, header_bytes)))
            pkt = lpkt()
            pkt.parse_header(header_bytes)
            logging.debug('payload size %d, reading'%pkt.payload_size)
            payload = sp.read(pkt.payload_size)
            logging.debug('got %d bytes'%len(payload))
            logging.debug('position: ' + ' '.join(map(lambda x:'%02x'%x, payload)))
            pos = lposition()
            pos.parse(payload)
            logging.debug('returning')
            return pkt, pos
            
    
if __name__=='__main__':
    with serial.Serial('/dev/ttyACM0', 115200, timeout=15) as s:
        print('poop')
        while True:
            try:
                header = None
                position = None
                try:
                    header, position = read_until_packet(s)
                except:
                    logging.debug("couldn't parse, likely just no data")
                position.rssi = header.rssi
                if header:
                    print(header)
                    print(position)
                    try:
                        db.insert_position(position)
                    except:
                        logging.warning("couldn't insert")
            except KeyboardInterrupt:
                s.close()
                sys.exit(0)
            except:
                print('no data?')
                    
