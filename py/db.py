#!/usr/bin/env python3

import mysql.connector, os, messages, json
from messages import *

def getconnection():
    host=os.environ.get('DBHOST', 'localhost')
    database=os.environ.get('MYSQL_DATABASE', 'perse')
    user=os.environ.get('MYSQL_USER', 'perse')
    password=os.environ.get('MYSQL_PASS','perse')
    return mysql.connector.connect(host=host,database=database,user=user,password=password)

schema="""create table if not exists position (_id integer primary key auto_increment,
 rssi integer,
 hour integer,
 minute integer,
 second integer,
 satellites integer,
 year integer,
 month integer,
 day integer,
 age integer,
 latitude double,
 longitude double,
 kmph double,
 course double,
 meters double);"""
def init():
    conn=getconnection()
    cu = conn.cursor()
    cu.execute(schema)
    conn.commit()
    conn.close()
init()

def insert_position(p):
    conn = getconnection()
    query = """insert into position (rssi, hour, minute, second, satellites, year, month, day, age, latitude, longitude, kmph, course, meters) values (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)"""
    cu = conn.cursor()
    cu.execute(query, (p.rssi, p.hour, p.minute, p.second, p.satellites, p.year, p.month, p.day, p.agemillis, p.lat, p.lng, p.kmph, p.course, p.meters))
    conn.commit()
    conn.close()

def row2position(r):
    logging.debug("double: %s"%str(r[9]))
    p = lposition()
    p.rssi = r[0]
    p.hour = r[1]
    p.minute = r[2]
    p.second = r[3]
    p.satellites = r[4]
    p.year = r[5]
    p.month = r[6]
    p.day = r[7]
    p.agemillis = r[8]
    p.lat = (r[9])
    p.lng = (r[10])
    p.kmph = (r[11])
    p.course = (r[12])
    p.meters = (r[13])
    return p

pos_columns = "rssi, hour, minute, second, satellites, year, month, day, age, latitude, longitude, kmph, course, meters"

def get_latest_position():
    conn = getconnection()
    query = "select %s from position order by _id desc limit 1"%pos_columns
    c = conn.cursor()
    c.execute(query)
    r = c.fetchone()
    p = row2position(r)
    conn.close()
    return p

def get_all_positions():
    conn = getconnection()
    query = "select %s from position order by _id desc"%pos_columns
    ps = []
    c = conn.cursor()
    c.execute(query)
    for row in c.fetchall():
        ps.append(row2position(row))
    conn.close()
    return ps

def get_positions(limit):
    conn = getconnection()
    query = "select %s from position order by _id desc limit %d"%(pos_columns, limit)
    ps = []
    c = conn.cursor()
    c.execute(query)
    for row in c.fetchall():
        ps.append(row2position(row))
    conn.close()
    return ps

if __name__=='__main__':
    #print(get_latest_position())
    print(json.dumps(db.get_all_positions(),
                     indent=4, sort_keys = True))
    

    
