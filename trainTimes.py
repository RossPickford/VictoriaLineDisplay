northbound = {"Walthamstow Central" : 105, 
              "Blackhorse Road" : 110, 
              "Tottenham Hale" : 100,
               "Seven Sisters" :  205, #need to potentiall extend due to increase time on platform 4
               "Finsbury Park" : 145,
               "Highbury & Islington" : 165,
               "King's Cross" : 90,
               "Euston" : 90,
               "Warren Street" : 100,
               "Oxford Circus" : 120,
               "Green Park" : 110,
               "Victoria" : 120,
               "Pimlico" : 85,
               "Vauxhall" : 130,
               "Stockwell" : 120}

southbound = {"Brixton" : 105,
              "Stockwell" : 135,
              "Vauxhall" : 90,
              "Pimlico" : 110,
              "Victoria" : 115,
              "Green Park" : 115,
              "Oxford Circus" : 95,
              "Warren Street" : 90,
              "Euston" : 90,
              "King's Cross" : 175,
              "Highbury & Islington" : 140,
              "Finsbury Park" : 200,
              "Seven Sisters" : 110,
              "Tottenham Hale" : 115,
              "Blackhorse Road" : 125}

def getStationTime(direction, station):
    if direction == "northbound":
        return northbound[station]
    else:
        return southbound[station]