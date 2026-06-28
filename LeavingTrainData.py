# Aiming to see when the location of a train is "leaving" or "left" - if the request gives the perspective from the station it is leaving, the one it is going to, and if that same train is also given with a different location (i.e between x and y)

import nextStation
import requests
import time

API_KEY = "580efc09100e43b6976feaefb4e33f12"
url = "https://api.tfl.gov.uk/Line/victoria/Arrivals"
params = {"app_key": API_KEY}


stationLeaving = {}
stationArriving = {}
stationBetween = {}
while (True):
    print("Gathering Data")    
    response = requests.get(url, params=params)
    data = response.json()
    for train in data:
        id = train.get("vehicleId", "Unknown")
        location = train.get("currentLocation", "No location available")
        if "Leaving" in location or "Left" in location:
            stationName = train.get("stationName", "No Station Available")
            stationName = stationName.replace(" Underground Station", "")
            if stationName in location:
                if id in stationLeaving:
                    stationLeaving[id] = location
                stationLeaving.update({id : location})
                print(f"Station Leaving : {id} : {location} : {stationName}")
            else:
                location = location.replace("Leaving ", "")
                location = location.replace("Left ", "")
                for train2 in data:
                    id2 = train2.get("vehicleId", "Unknown")
                    if id2 == id:
                        direction = train.get("towards", "No Direction Available")
                        if direction == "Brixton":
                            nextStation = nextStation.outhbound[location]
                        elif direction == "Walthamstow Central":
                            nextStation = nextStation.northbound[location]
                        stationName2 = train.get("stationName", "No Station Available")
                        stationName2 = stationName2.replace(" Underground Station", "")
                        location2 = train.get("currentLocation", "No location available")
                        if "Leaving" in location2 or "Left" in location2:
                            if stationName2 == nextStation:
                                if id not in stationArriving:
                                    stationArriving.update({id : location2})
                                    print(f"Station Arriving : {id} : {location} : {stationName}")
                        elif "Between" in location:
                            if id not in stationBetween:
                                stationBetween.update({id : location})
                                print(f"Station Between : {id} : {location} : {stationName}")

    print(f"Station Leaving Count: {len(stationLeaving)} | Station Arriving Count: {len(stationArriving)} | Station Between Count: {len(stationBetween)}")

    time.sleep(30)