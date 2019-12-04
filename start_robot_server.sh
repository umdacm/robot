#!/bin/bash

ssh mobility@10.0.0.2 "cd libvisca-1.1.0/examples/; ./visca_cli" &
ssh mobility@10.0.0.2 "name -i; base" &  # Start drive server
ssh mobility@10.0.0.2 "./mobility-b-1.1.8nb/examples/mby/rotate-test/rotate-test" &
python raspi-proxy-server/server.py


