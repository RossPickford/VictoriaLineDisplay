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

station_numerical = { "Brixton" : 124,
                "Stockwell" : 116,
                "Vauxhall" : 108,
                "Pimlico" : 100,
                "Victoria" : 92,
                "Green Park" : 84,
                "Oxford Circus" : 76,
                "Warren Street" : 68,
                "Euston" : 60,
                "Kings Cross St. Pancras" : 52,
                "Highbury & Islington" : 44,
                "Finsbury Park" : 36,
                "Seven Sisters" : 28,
                "Tottenham Hale" : 20,
                "Blackhorse Road" : 12,
                "Walthamstow Central" : 4}

# import subprocess
# subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/", "-o", "trains.json"])
# subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUBXN", "-o", "southbound.json"])
# subprocess.run(["curl", "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUWWL", "-o", "northbound.json"])

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

totalTime_init = time.time() - totalTime_init
print(f"\nPre-initialisation took {totalTime_init} seconds")


def getTrains():
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

        temp_trainData = {}
        for train in data:
            dataCollection_start = time.time()

            id = train.get("vehicleId", "No vehicle Id")
            if not id.isdigit():
                print("Invalid train id")
                continue
            if id in train_data.keys():
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

            if station != station_name:
                continue

            direction = train.get("towards","No direction")

            direction = -1 if direction == "Walthamstow Central" else 1
            id_int = int(id)
            time_int = int(timeToStation)

            if id_int not in temp_trainData.keys():
                temp_trainData.update({id_int : [direction, station_numerical[station], time_int, state]})
            elif temp_trainData[id_int][1] != station_numerical[station]:
                print(f"Error 01: Different Location for train {id_int} - saved Station {temp_trainData[id_int][1]} | new station {station}")
            elif time_int < temp_trainData[id_int][2]: 
                    temp_trainData[id_int][2] = time_int

            totalTime_trainDataCollection += time.time() - dataCollection_start

        train_data.update(temp_trainData)
        count += 1

    print(f"Requests took {totalTime_request} seconds")
    print(f"Data collection took {totalTime_trainDataCollection} seconds\n")
    print("\n==============Trains==============\n")

    list_data = []
    for id, loc in train_data.items():
        print(f"ID: {id} | {loc[0]} | {"next" if loc[3] == "moving" else "current"} Stop: {loc[1]} | time to: {loc[2]}")
        list_data.append([id, loc[0], loc[1], loc[2], loc[3]])
    
    return list_data





    
