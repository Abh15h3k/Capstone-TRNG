# Capstone-TRNG

## Round Trip Time
Compile rtt `make`\
Execute binary with default options `./rtt`\
Execute binary with custom number size `./rtt -n {8, 16, 32, 64}`

## Android Sensors
A compiled APK is provided in the `android-sensors` along with the android studio project.

## Packet Sniffer
Install the pcap library if not already present `sudo apt-get install libpcap-dev`\
Compile sniffer.c `gcc sniffer.c -lpcap -o sniffer`\
Execute the binary with default options i.e with no filters and sniff on any device `sudo ./sniffer`\
Execute with filters and on a particular device `sudo ./sniffer -p "dst host 192.168.1.8" -d "wlo1"`
