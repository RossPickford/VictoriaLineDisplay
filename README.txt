Terminal commands to run for the wifi dongle - this allows it to run without idling:

echo 'ACTION=="add", SUBSYSTEM=="usb", ATTR{idVendor}=="148f", ATTR{idProduct}=="7601", TEST=="power/control", ATTR{power/control}="on"' | sudo tee /etc/udev/rules.d/99-mt7601u-no-suspend.rules

sudo udevadm control --reload-rules && sudo udevadm trigger

sudo iw dev wlan0 set power_save off

sudo tee /etc/NetworkManager/dispatcher.d/99-wlan-powersave-off << 'EOF'
#!/bin/bash
if [ "$1" = "wlan0" ] && [ "$2" = "up" ]; then
    /sbin/iw dev wlan0 set power_save off
fi
EOF
sudo chmod +x /etc/NetworkManager/dispatcher.d/99-wlan-powersave-off

sudo apt update && sudo apt install firmware-misc-nonfree
