#!/usr/bin/env python

"""{
  "type": "Feature",
  "geometry": {
    "type": "Point",
    "coordinates": [125.6, 10.1]
  },
  "properties": {
    "name": "Dinagat Islands"
  }
}"""

def position2geojsonobject(p):
    return {'type': 'Feature',
            'geometry': {
                'type': 'Point',
                'coordinates': [p.lng, p.lat]
            },
            'properties': {
                'name': 'rssi %d'%p.rssi
            }
            }
            
