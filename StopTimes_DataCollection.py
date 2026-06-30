from requests import get
import time

#"Oxford Circus" : "940GZZLUOXC"
#"Warren Street" : "940GZZLUWRR"

#trying to find out if the predicted time at a station is less than the request interval time

API_KEY = "580efc09100e43b6976feaefb4e33f12"
base_url = "https://api.tfl.gov.uk/Line/victoria/Arrivals/"
base_url += "940GZZLUOXC"
params = {"app_key": API_KEY}

stationTime = {}
intervals = {}
anomalies = []

test = ["1", "2", "3", "4"]
print(f"Here are the numbers: {", ".join(test)}")

while (True):
    print("gathering info")
    request = get(base_url, params=params)
    data = request.json()
    ids = []
    tempStationTime = {}
    for d in data:
        location = d.get("currentLocation", "No location available")
        if "At Oxford Circus" in location or "At Platform" in location:
            t = d.get("timeToStation", "No time available")
            id = d.get("vehicleId", "No id")
            
            if id not in ids:
                ids.append(id)
            
            if t < 30:
                if id not in tempStationTime:
                    tempStationTime.update({id : t})
                elif tempStationTime[id] > t:
                    tempStationTime[id] = t

    for id in tempStationTime.keys():
        if id in stationTime:
            stationTime[id].append(tempStationTime[id])
        else:
            stationTime[id] = [tempStationTime[id]]

    for id, t in stationTime.items():
        if id in intervals:
            intervals[id] += 1
        else:
            intervals.update({id : 0})

        if id not in ids:
            if len(t) > 1:
                print(f"Train: {id} | Times: {", ".join(str(x) for x in t)}")
            else:
                print(f"Train: {id} | Time: {t[0]}")
        else:
            print(f"Train {id} still at station")
    time.sleep(30)

