import sys
import json
sys.path.append('C:\\Users\\pickford ross\\AppData\\Local\\Programs\\Python\\Python314\\Lib\\site-packages');

import requests

print("we are here")

API_KEY = "580efc09100e43b6976feaefb4e33f12"
url = "https://api.tfl.gov.uk/Line/victoria/Arrivals"
params = {"app_key": API_KEY}

response = requests.get(url, params=params)
data = response.json()

trainData = []

print("Southbound Trains: ")
for train in data:
    vehicle_id = train.get("vehicleId", "Unknown")
    stationName = train.get("stationName", "No Station Available")
    stationName.replace(" Underground Station", "")
    print(stationName)
    location = train.get("currentLocation", "No location available")
    time = train.get("timeToStation", "Unknown")
    if stationName in location:
        trainData.append([vehicle_id, stationName, time])

for train in trainData:
    print(f"Train {train[0]} → {train[1]} | time to station: {train[2]}")