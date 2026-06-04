#!/bin/bash
cd /home/iammira/ws_capa
bash install/setup.bash >/dev/null 2>&1
echo "Exit code: $?"
echo "AMENT_CURRENT_PREFIX=$AMENT_CURRENT_PREFIX"
