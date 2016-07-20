#!/bin/sh

./go.sh

screen -dmS "JLink" JLinkExe -device nRF51422_xxAC -speed 4000
JLinkRTTClient | python analyse_location.py 
