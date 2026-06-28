#### Project Notes

There are Three main aspects to this project - titled below.

## Hardware

This includes all electronic components required for this build.

Raspberry Pi 2B - I have, hopefully it is powerful enough to handle this project.

[LED 64x32 Panel](https://www.aliexpress.com/item/1641364020.html?spm=a2g0o.productlist.main.2.67581e7529ZiEO&algo_pvid=959c482b-d9e0-4408-91c3-fb04c398dbb8&algo_exp_id=959c482b-d9e0-4408-91c3-fb04c398dbb8-1&pdp_ext_f=%7B%22order%22%3A%2244%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21GBP%219.99%219.99%21%21%2113.02%2113.02%21%402103867617808719017192515eb10d%2153628021726%21sea%21UK%210%21ABX%211%210%21n_tag%3A-29910%3Bd%3A448c2d59%3Bm03_new_user%3A-29895&curPageLogUid=I8kDROHlbfQd&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1641364020%7C_p_origin_prod%3A) - from AliExpress

[Here is one from Adafruit](https://thepihut.com/products/adafruit-64x32-rgb-led-matrix-3mm-pitch) - It's more trusted, but pricier

[LED Matrix Bonnet](https://thepihut.com/products/adafruit-triple-led-matrix-bonnet-for-raspberry-pi-for-hub75) - This also mentions the LED Matrix API


## Software

[LED Matrix API](https://github.com/hzeller/rpi-rgb-led-matrix) - the library is written in C++, but there is a wrapper API in C and Python

[Here is the Python Wrapper for it](https://github.com/hzeller/rpi-rgb-led-matrix/tree/master/bindings/python) - Will need to look into how to compile it/ install it

~~The aim is to embed Python to call the HTTP request for all the data from the Line.~~

~~It seems much more simplier to just use python. The matrix API also has a python wrapper.~~ 

It seems I have now embedded Python into C++. Since the matrix API is built in C++, I might as well use it. The next aim will be to retrieve data from Python into C++.

A quick note but here is some information that could potentially allow this project to be purely C++:

- (HTTP Request C++ API)[https://www.youtube.com/watch?v=RzdM54i7buY]
- (JSON Parser API)[https://rapidjson.org/md_doc_dom.html#Parsing]
- (Potentially using 2 Raspberry Pis vis Serial Ports)[https://www.waveshare.com/wiki/Raspberry_Pi_Tutorial_Series:_Serial?srsltid=AfmBOorZFqnuxsJR4YX9LPxRoMZamXQ3WljWk7ybPLJ6dZkj5PeAE_zk]

[Hre is the timetable for stations on victoria](https://tfl.gov.uk/corporate/publications-and-reports/working-timetables)

### Basic Request Logic

Using the requests module.

Currently The dictionary stores the Key as the vehicle ID, and then the value is a list where the first value is the next station, the second is the direction, and the third is the time till the next station

**One issue that I have already thought of is that my form of sorting trains into the dictionary it doesn't look at the other words in location. If it says "Leaving {station}" - it will always assume you are arriving to a station.**