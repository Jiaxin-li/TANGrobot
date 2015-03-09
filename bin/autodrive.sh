#!/bin/bash 
echo "auto control."
xterm -e ./supercomponent --cid=111 & xterm -e ./cockpit --cid=111 & xterm -e ./vehicle --cid=111 --freq=10 & xterm -e ./camgen --cid=111 & xterm -e ./irus --cid=111 --freq=10 & xterm -e ./lanedetector --cid=111 --freq=10 & xterm -e ./driver --cid=111 --freq=10
