import subprocess
import time
from requests import get, Timeout


trains = {}

while (1):
    API_KEY = "580efc09100e43b6976feaefb4e33f12"
    base_url = "https://api.tfl.gov.uk/Line/victoria/Arrivals/"
    params = {"app_key": API_KEY}
    
    request = get(base_url, params=params)

    data = request.json()

    for train in data:
        id = train.get("vehicleId", "no vehicle ID")
        station_name = train.get("stationName", "no station found")
        station_name = station_name.replace(" Underground Station", "")

        if not id.isdigit():
            print("Invalid train id")
            continue

        location = train.get("currentLocation", "Location unknown")
        station = location            
        timeToStation = 0
        state = 0

        if "At Platform" in location:
            if id not in trains:
                trains.update({id : [station_name, time.time(), "at platform"]})
        elif "At" not in location:
            if id in trains and trains[id][2] == "at platform":
                trains[id][1] = time.time() - trains[id][1]
                trains[id][2] = "done"


    print("\n------------------------\n")
    
    for id,t in trains.items():
        if t[2] == "done":
            print(f"id: {id}, station: {t[0]}, time to depart: {t[1]}")            

        


    
    time.sleep(30)