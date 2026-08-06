import subprocess
subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/", "-o", "trains.json"])
# subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUBXN", "-o", "southbound.json"])
# subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUWWL", "-o", "northbound.json"])

station_index = { 
                "Walthamstow Central" : 0,
                "Blackhorse Road" : 1,
                "Tottenham Hale" : 2,
                "Seven Sisters" : 3,
                "Finsbury Park" : 4,
                "Highbury & Islington" : 5,
                "King's Cross St. Pancras" : 6,
                "Kings Cross St. Pancras" : 6,
                "Euston" : 7,
                "Warren Street" : 8,
                "Oxford Circus" : 9,
                "Green Park" : 10,
                "Victoria" : 11,
                "Pimlico" : 12,
                "Vauxhall" : 13,
                "Stockwell" : 14,
                "Brixton" : 15
                }

import time
totalTime_init = time.time()

from sys import path
import link
path.append(link.getRequestsAddress())

from requests import get
import nextStation

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

API_KEY = "580efc09100e43b6976feaefb4e33f12"
base_url = "https://api.tfl.gov.uk/Line/victoria/Arrivals/"
params = {"app_key": API_KEY}

totalIds = {}

def requestTrainData():
    totalTime_request = time.time()
    request = get(base_url, params=params)
    data = request.json()
    totalTime_request = time.time() - totalTime_request

    train_data = {}
    totalTime_trainDataCollection = time.time()
    for train in data:
        id = train.get("vehicleId", "no vehicle ID")
        station_name = train.get("stationName", "no station found")
        station_name = station_name.replace(" Underground Station", "")

        if int(id) not in totalIds:
            location = train.get("currentLocation", "no location found")
            totalIds.update({int(id) : location})

        if not id.isdigit():
            print("Invalid train id")
            continue

        location = train.get("currentLocation", "Location unknown")
        station = location            
        timeToStation = 0
        state = 0

        if "Between" in location:
            station = getStationFromLocation(location)
            state = 1

        elif "Approaching" in location:
            station = location.replace("Approaching ", "")
            state = 1

        elif "At Platform" in location:
            station = station_name

        elif "At" in location:
            station = location.replace("At ", "")
            if "Seven Sisters" in station:
                station = station.replace(" Platform 5", "")

        elif "Departing" in location or "Departed" in location or "Left" in location: 
            station = station.replace("Departed ", "")
            station = station.replace("Departing ", "")
            station = station.replace("Left ", "")
            print(f"station name is now: {station}")

            destination = train.get("towards", "no destination")
            a = 1 if destination == "Brixton" else -1

            index = station_index[station]
            station = list(station_index.keys())[index + a]

            state = 1
            # print(f"{id} | {destination} : {location} : {station} : {station_name}")


        elif "Area" in location and "Brixton" in location:
            station = "Brixton"
            state = 1

        else:
            print(f"Unknown Location Value: {location} for train: {id} at {station_name}")
            continue

        if station == "Highbury & Isl":
            station = "Highbury & Islington"
        elif station == "Kings Cross St. P" or station == "Kings Cross St. Pancras":
            station = "King's Cross St. Pancras"


        if station != station_name and state != 0 and "At" not in location:
            print(f" {id} station: {station} | current station: {station_name}")
            continue

        if state == 1:
            timeToStation = train.get("timeToStation", "No time found")

        time_int = int(timeToStation)

        direction = train.get("towards","No direction")
        if direction == "Brixton":
            direction = 1
        elif direction == "Walthamstow Central" or direction == "Seven Sisters":
            direction = -1
        else:
            print(f"{id} has an unknown different destination: {direction}")
            continue

        id = int(id)
        if id not in train_data.keys():
            train_data.update({id : [direction, station_index[station], time_int, state, location]})
        else:
            if station_index[station] != train_data[id][1]:
                t_stationIndex = train_data[id][1]
                nextStation_i = station_index[station] + direction
                if (direction == 1 and nextStation_i > t_stationIndex) or (direction == -1 and nextStation_i < t_stationIndex):
                    train_data[id][0] = direction
                    train_data[id][1] = station_index[station]
                    train_data[id][2] = time_int
                    train_data[id][3] = state
                    train_data[id][4] = location
            elif time_int < train_data[id][2]:
                train_data[id][2] = time_int

    totalTime_trainDataCollection = time.time() - totalTime_trainDataCollection

    # for id in totalIds.keys():
        # if id not in train_data.keys():

    print(f"request took: {totalTime_request}s")
    print(f"data collection took: {totalTime_trainDataCollection}s")
    print(f"length of total request ids: {len(totalIds)}")
    print(f"length of my request id list: {len(train_data)}")

    for id, loc in totalIds.items():
        if id not in train_data.keys():
            print(f"{id} at {loc} - not in train_data")

    list_data = []
    print("=======================================================")
    for id, data in train_data.items():
        print(f"{id} | direction: {data[0]} | station: {data[1]} | location: {data[4]} | time to station: {data[2]}")

        list_data.append([id, data[0], data[1], data[2], data[3]])

    test_list = []
    test_list.append([1, 1, 3, 4, 5])
    test_list.append([2, -1, 5, 6, 8])
    return list_data
