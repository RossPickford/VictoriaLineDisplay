# import subprocess
# subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/", "-o", "trains.json"])
# subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUBXN", "-o", "southbound.json"])
# subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUWWL", "-o", "northbound.json"])

import time
totalTime_init = time.time()

station_ids = { "Brixton" : "940GZZLUBXN",
                "Stockwell" : "940GZZLUSKW",
                "Vauxhall" : "940GZZLUVXL",
                "Pimlico" : "940GZZLUPCO",
                "Victoria" : "940GZZLUVIC",
                "Green Park" : "940GZZLUGPK",
                "Oxford Circus" : "940GZZLUOXC",
                "Warren Street" : "940GZZLUWRR",
                "Euston" : "940GZZLUEUS",
                "Kings Cross St. Pancras" : "940GZZLUKSX",
                "Highbury & Islington" : "940GZZLUHAI",
                "Finsbury Park" : "940GZZLUFPK",
                "Seven Sisters" : "940GZZLUSVS",
                "Tottenham Hale" : "940GZZLUTMH",
                "Blackhorse Road" : "940GZZLUBLR",
                "Walthamstow Central" : "940GZZLUWWL"}

stop_id = ["", "940GZZLUBXN", "940GZZLUWWL"]

from sys import path
import link
path.append(link.getRequestsAddress())

from requests import get
import nextStation

API_KEY = "580efc09100e43b6976feaefb4e33f12"
base_url = "https://api.tfl.gov.uk/Line/victoria/Arrivals/"
params = {"app_key": API_KEY}

totalIds = {}

request = get(base_url, params=params)
data = request.json()

for train in data:
    id = train.get("vehicleId", "no vehicle ID")

    if id not in totalIds:
        location = train.get("currentLocation", "no location found")
        totalIds.update({id : location})

print(f"length of total request ids: {len(totalIds)}")

def getStationFromLocation(location):
    buf = []
    and_count = 0
    for i in range(len(location)):
        buf.append(location[i])
        if location[i] == 'a' and and_count == 0:
            and_count += 1
        elif location[i] == 'n' and and_count == 1:
            and_count += 1
        elif location[i] == 'd' and and_count == 2:
            buf.append(location[i + 1])
            break
        else:
            and_count = 0
    remove = "".join(buf)
    return location.replace(remove, "")

train_data = {}
totalTime_trainDataCollection = 0
totalTime_request = 0
count = 0 #used to track where in the loop we are
for station_name, stop_id in station_ids.items():
    request_start = time.time()

    url = base_url + stop_id
    request = get(url, params=params)
    data = request.json()

    totalTime_request += time.time() - request_start

    for train in data:
        dataCollection_start = time.time()
        id = train.get("vehicleId", "No vehicle Id")
        if not id.isdigit():
            print("Invalid train id")
            continue
        if id in train_data.keys() and train_data[id][0] == station_name:
            train_data[id][2] = int(train.get("timeToStation", "No time found"))
            continue

        location = train.get("currentLocation", "Location unknown")
        station = location            
        timeToStation = 0
        state = 0
        if "Between" in location:
            station = getStationFromLocation(location)
            state = 1
            timeToStation = train.get("timeToStation", "No time given")

        elif "Approaching" in location:
            station = location.replace("Approaching ", "")
            state = 1
            timeToStation = train.get("timeToStation", "No time given")
            timeToStation = int(timeToStation) if timeToStation != "No time given" else -1

        elif "At Platform" in location:
            station = station_name
            state = 2

        elif "At" in location:
            station = location.replace("At ", "")
            state = 2
            if "Seven Sisters" in station:
                station = station.replace(" Platform 5", "")

        elif "Departing" in location or "Departed" in location or "Left" in location: 
            destination = train.get("towards", "no destination")
            a = 1 if destination == "Brixton" else -1
            previousStation = list(station_ids.keys())[count + a]
            # print(f"{id} | {destination} : {location} : {previousStation} : {station_name}")
            if previousStation in location:
                station = station_name
                state = 1
                timeToStation = train.get("timeToStation", "No time given")
            else:
                continue

        elif "Area" in location and "Brixton" in location:
            station = "Brixton"
            state = 1
        else:
            print(f"Unknown Location Value: {location} for train: {id} at {station_name}")

        if station == "Highbury & Isl":
            station = "Highbury & Islington"
        elif station == "Kings Cross St. P":
            station = "Kings Cross St. Pancras"

        # if station != station_name:
        #     continue

        direction = train.get("towards","No direction")
        direction = -1 if direction == "Walthamstow Central" else 1

        # id_int = int(id)
        time_int = int(timeToStation)

        if id not in train_data.keys():
            train_data.update({id : [station, location, 0]})

    count += 1

print(f"length of my request id list: {len(train_data.keys())}")

for id, loc in totalIds.items():
    if id not in train_data.keys():
        print(f"{id} at {loc} - not in train_data")
    else:
        print(f"{id} | station: {train_data[id][0]} | location: {train_data[id][1]} | time to station: {train_data[id][2]}")
    
