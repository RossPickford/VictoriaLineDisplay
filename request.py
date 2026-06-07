import requests

API_KEY = "580efc09100e43b6976feaefb4e33f12"
url = "https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUBXN"
params = {"app_key": API_KEY}

response = requests.get(url, params=params)
data = response.json()

print("Southbound Trains: ")
for train in data:
    vehicle_id = train.get("vehicleId", "Unknown")
    location = train.get("currentLocation", "No location available")
    destination = train.get("destinationName", "Unknown")
    time = train.get("timeToStation", "Unknown")
    print(f"Train {vehicle_id} → {location} | time to end station: {time}")