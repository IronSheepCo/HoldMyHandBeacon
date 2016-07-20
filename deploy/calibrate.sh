#!/bin/sh

screen -dmS "JLink" JLinkExe -device nRF51422_xxAC -speed 4000
JLinkRTTClient > tmp.txt &
sleep 6
killall JLinkRTTClient
