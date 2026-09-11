#!/bin/bash
set -e

# Source ROS 2 base environment
source "/opt/ros/humble/setup.bash"

# Source local workspace setup if built
if [ -f "/ws/install/setup.bash" ]; then
    source "/ws/install/setup.bash"
fi

exec "$@"
