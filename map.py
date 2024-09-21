import fiona
import json
import numpy as np
import requests

from madcad.mesh import Wire
from madcad.triangulation import *

utf8stdout = open(1, 'w', encoding='utf-8', closefd=False)

def countryBanned(code):
    return code in ['KAS', 'LIE', 'AND']

def countryExists(code):
    return not code in ['ATA', 'VAT', 'SMR', 'MCO', 'NRU', 'SGP', 'ATG', 'BHR', 'BRB', 'DMA', 'FSM', 'GRD', 'KIR', 'KNA', 'LCA', 'MDV', 'MHL', 'MLT', 'PLW', 'SGP', 'SYC', 'TON', 'TUV', 'VCT', 'WSM', 'STP']

def getBoundingBox(vert):
    vmax = [-1e9, -1e9]
    vmin = [1e9, 1e9]
    for v in vert:
        vmax[0] = max(vmax[0], v[0])
        vmax[1] = max(vmax[1], v[1])
        vmin[0] = min(vmin[0], v[0])
        vmin[1] = min(vmin[1], v[1])
    
    return (tuple(vmin), tuple(vmax))

def triangulate(vert):
    newv = map(lambda e: [e[0], e[1], 0], vert)
    
    outline = Wire(newv)

    mesh = triangulation_outline(outline)

    assert(len(mesh.points) == len(vert))

    return mesh.faces

def earthDistance(lat1, lon1, lat2, lon2):
    r = 6371
    p = pi / 180
    a = 0.5 - cos((lat2-lat1)*p)/2 + cos(lat1*p) * cos(lat2*p) * (1-cos((lon2-lon1)*p))/2
    return 2 * r * asin(sqrt(a))

MIN_CITY_DISTANCE = 99
def removeCloseCities(unsortedCities):
    cities = sorted(unsortedCities, reverse=True, key=lambda x: (x['capital'], x['population']))

    okCities = [i for i in range(0, len(cities))]
    for i in okCities:
        (lat1, lon1) = cities[i]['coords']
        for j in okCities:
            if j <= i: 
                continue
            (lat2, lon2) = cities[j]['coords']
            if earthDistance(lat1, lon1, lat2, lon2) < MIN_CITY_DISTANCE and not cities[j]['capital']:
                okCities.remove(j)
    
    return [cities[i] for i in okCities]

def airportCities():
    shape = fiona.open('resources/ne_10m_populated_places/ne_10m_populated_places.shp')
    cities = []

    for feat in iter(shape):
        city = {
            'name': feat.properties['NAMEASCII'],
            'population': feat.properties['POP_MAX'],
            'capital': bool(feat.properties['ADM0CAP']),
            'coords': feat.geometry['coordinates'],
            'country': feat.properties['ADM0_A3']
        }

        if city['name'] in ['Gaza', 'Juba', 'Bir Lehlou', 'Stanley', 'Nuuk', 'Torshavn', 'Tel Aviv-Yafo']:
            city['capital'] = True

        if city['name'] == 'Jerusalem':
            city['capital'] = False

        if city['population'] > 50000 or city['capital']: # Less than 50k people isn't a city
            cities.append(city)

    cities = removeCloseCities(cities)

    countries = {}
    for cit in cities:
        c = cit['country']
        del cit['country']
        if c in countries:
            countries[c] = countries[c] + [cit]
        else:
            countries[c] = [cit]

    return countries

def countriesMesh():
    shape = fiona.open('resources/ne_50m_admin_0_countries/ne_50m_admin_0_countries.shp')
    
    countries = {}
    vertices = []
    triangles = []
    pairs = 0
    tris = 0
    for feat in iter(shape):
        polygons = []
        name = str(feat.properties['ADMIN'])
        code = feat.properties['ADM0_A3']
        iso = feat.properties['ISO_A2_EH']
        if not countryExists(code):
            continue
        elif code == 'SDS':
            code = 'SSD'
        elif code == 'CYN':
            code = 'CYP'
            name = 'Cyprus'
            iso = 'CY'
        elif feat.properties['SOV_A3'] in ['GB1', 'US1', 'FR1', 'CH1', 'AU1', 'NL1', 'NZ1', 'FI1'] and code not in ['FLK']: #Why island think they are independent?
            tradCode = {'GB1': 'GBR', 'US1': 'USA', 'FR1': 'FRA', 'CH1': 'CHN', 'AU1': 'AUS', 'NL1': 'NLD', 'NZ1': 'NZL', 'FI1': 'FIN'}
            tradName = {'GB1': 'United Kingdom', 'US1': 'United States of America', 'FR1': 'France', 'CH1': 'China', 'AU1': 'Australia', 'NL1': 'Netherlands', 'NZ1': 'New Zealand', 'FI1': 'Finland'}
            tradIso = {'GB1': 'GB', 'US1': 'US', 'FR1': 'FR', 'CH1': 'CN', 'AU1': 'AU', 'NL1': 'NL', 'NZ1': 'NZ', 'FI1': 'FI'}
            code = tradCode[feat.properties['SOV_A3']]
            name = tradName[feat.properties['SOV_A3']]
            iso = tradIso[feat.properties['SOV_A3']]

        if feat.geometry['type'] == 'MultiPolygon':
            polygons = [e[0] for e in feat.geometry['coordinates']]
        else:
            polygons = feat.geometry['coordinates']

        polygons = [p for p in polygons if len(p) > 8]

        polygonInfo = {
            "triangleIndex": (0, 0),
            "vertexIndex": (0, 0),
            "box": ((0.0, 0.0), (0.0, 0.0))
        }
        polygonIndexes = [polygonInfo.copy() for _ in polygons]

        for i, v in enumerate(polygons):
            polygonIndexes[i]['box'] = getBoundingBox(v)

            tri = triangulate(v)
            tri = list(map(lambda t: [t[0] + pairs, t[1] + pairs, t[2] + pairs], tri))
            triangles.extend(tri)
            polygonIndexes[i]['triangleIndex'] = (tris, tris + len(tri))
            tris = tris + len(tri)
            print(tris)

            vertices.extend(v)
            polygonIndexes[i]['vertexIndex'] = (pairs, pairs + len(v))
            pairs = pairs + len(v)

        if not code in countries:
            countries[code] = {
                'banned': countryBanned(code),
                'name': name,
                'iso': iso,
                'mesh': polygonIndexes,
            }
        else:
            countries[code]['mesh'] = countries[code]['mesh'] + polygonIndexes

    return (countries, vertices, triangles)

def main():
    airportsRaw = airportCities()
    (countries, vertices, triangles) = countriesMesh()
    air = {}

    for c in countries:
        if not countries[c]['banned']:
            air[c] = airportsRaw[c]

    with open('resources/countries.json', 'w', encoding='utf-8') as f:
        json.dump(countries, f)

    with open('resources/airports.json', 'w', encoding='utf-8') as f:
        json.dump(air, f)

    vertNP = np.array(vertices, 'float32')
    triNP = np.array(triangles, 'int32')

    print(len(vertNP))
    print(len(triNP))
    with open('resources/mesh.bin', 'wb') as f:
        np.int32(len(vertNP)).tofile(f)
        np.int32(len(triNP)).tofile(f)
        vertNP.tofile(f)
        triNP.tofile(f)

def getFlags():
    countries = {}
    with open('resources/countries.json', 'r') as c:
        countries = json.load(c)

    flags = {}
    for i in countries:
        code = countries[i]['iso'].lower()
        if i == 'SOL': code = 'somaliland'
        elif i == 'KAS': code = 'pk-jk'
        url = f"https://hatscripts.github.io/circle-flags/flags/{code}.svg"
        response = requests.get(url)
        if response.status_code != 200:
            print("no", countries[i]['name'])
        else:
            flags[i] = response.text
    
    with open('resources/flags.json', 'w', encoding='utf-8') as f:
        json.dump(flags, f)

if __name__ == '__main__':
    main()
    getFlags()
