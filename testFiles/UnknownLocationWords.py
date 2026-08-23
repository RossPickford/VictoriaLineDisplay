# Aiming to see when the location of a train is "leaving" or "left" - if the request gives the perspective from the station it is leaving, the one it is going to, and if that same train is also given with a different location (i.e between x and y)

stations = ["Walthamstow Central",
              "Blackhorse Road", 
              "Tottenham Hale",
              "Seven Sisters",
              "Finsbury Park",
              "Highbury & Islington",
              "King's Cross St. Pancras",
              "Euston",
              "Warren Street",
              "Oxford Circus",
              "Green Park",
              "Victoria",
              "Pimlico",
              "Vauxhall",
              "Stockwell",
              "Brixton"]

import requests
import time

API_KEY = "580efc09100e43b6976feaefb4e33f12"
url = "https://api.tfl.gov.uk/Line/victoria/Arrivals"
params = {"app_key": API_KEY}

knownWords = ["At", "Between", "Departed", "Approaching", "Area", "Left", "Departing"]
UnknownWords = {}


while (True):
    print("Gathering Data")    
    response = requests.get(url, params=params)
    data = response.json()
    for train in data:
        id = train.get("vehicleId", "No id")
        if id in UnknownWords:
            continue
        location = train.get("currentLocation", "No location available")
        if any(s in location for s in knownWords):
            continue
        new_word = ""
        if id not in UnknownWords:
            UnknownWords.update({id : location})
        else:
            UnknownWords[id] = location

    for id, location in UnknownWords.items():
        print(f"New Word: {id} | Count: {location}")

    time.sleep(30)