# ==============================================================================
# Stage 1: Base Image (Common ROS 2 Humble + Simulation Dependencies)
# ==============================================================================
FROM osrf/ros:humble-desktop-full AS base

# Prevent interactive prompts during apt installation
ENV DEBIAN_FRONTEND=noninteractive

# Install essential system & ROS 2 dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    libserial-dev \
    python3-pip \
    python3-venv \
    python3-dev \
    ros-humble-moveit \
    ros-humble-ros2-control \
    ros-humble-ros2-controllers \
    ros-humble-ros-gz \
    ros-humble-ign-ros2-control \
    && pip3 install --no-cache-dir ask-sdk-core flask-ask-sdk Flask pyserial \
    && rm -rf /var/lib/apt/lists/*

# ==============================================================================
# Stage 2: Dev Environment (Used by VS Code DevContainers)
# ==============================================================================
FROM base AS dev

ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create non-root user and grant sudo access
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && apt-get update && apt-get install -y --no-install-recommends sudo git \
    && echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME \
    && rm -rf /var/lib/apt/lists/*

USER $USERNAME
WORKDIR /home/$USERNAME

RUN echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc \
    && echo "if [ -f /home/developer/workspace/arduinobot_ws/install/setup.bash ]; then source /home/developer/workspace/arduinobot_ws/install/setup.bash; fi" >> ~/.bashrc

# ==============================================================================
# Stage 3: Standalone Simulation Demo (Pre-built sibling image)
# ==============================================================================
FROM base AS sim

WORKDIR /ws

# Copy workspace source code into container
COPY arduinobot_ws/src /ws/src

# Update rosdep, install dependencies, and build workspace
RUN find /ws/src -name "*.py" -exec chmod +x {} + && \
    apt-get update && \
    rosdep update && \
    rosdep install --from-paths src --ignore-src -r -y && \
    /bin/bash -c "source /opt/ros/humble/setup.bash && colcon build --symlink-install" && \
    rm -rf /var/lib/apt/lists/*

# Copy entrypoint script
COPY docker/ros_entrypoint.sh /ros_entrypoint.sh
RUN chmod +x /ros_entrypoint.sh

ENTRYPOINT ["/ros_entrypoint.sh"]
CMD ["ros2", "launch", "arduinobot_bringup", "simulated_robot.launch.py"]
