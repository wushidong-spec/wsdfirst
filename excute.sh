#!/bin/bash
for i in $(seq 1 3); do   
    nohup ./rpcclient &   
    sleep 5  
done
