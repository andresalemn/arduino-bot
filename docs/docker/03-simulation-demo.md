# Simulation Demo (Standalone Image)

Want to see what Arduinobot does without installing ROS 2, Gazebo, or building the physical arm? This is the fastest path — a pre-built Docker image with everything baked in.

> This image is for **evaluation only** — it's not meant for development. If you want to edit code and iterate, see the [Dev Container setup](./02-devcontainer-setup.md) instead.

## What you get

Running this image launches the Arduinobot simulation stack (Gazebo + MoveIt 2 + RViz2) with the same `simulated_robot.launch.py` used in development — no physical hardware, no wiring, no calibration needed. You'll see the 4-DOF arm model spawned in Gazebo and can interact with it through MoveIt 2's motion planning.

## Prerequisites

- [Docker](https://docs.docker.com/get-docker/) installed and running
- A way to display GUI apps from a container on your machine:
  - **Linux:** works out of the box with X11 forwarding (see run command below).
  - **Windows 11 (or updated Windows 10):** WSLg handles this automatically through Docker Desktop — no extra setup.
  - **Older Windows / macOS:** you'll need a separate X server (e.g. [VcXsrv](https://sourceforge.net/projects/vcxsrv/) on Windows, [XQuartz](https://www.xquartz.org/) on macOS) and to set the `DISPLAY` environment variable manually. See [troubleshooting](./04-troubleshooting.md#gui-apps-on-windows).

## Running the demo

```bash
docker pull ghcr.io/andresalemn/arduinobot-sim:latest

docker run -it --rm \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  ghcr.io/andresalemn/arduinobot-sim:latest
```

> **Note:** the image name/tag above is a placeholder until the image is published — see [`01-overview.md`](./01-overview.md) for build status. Until then, you can build and run it locally:
> ```bash
> git clone https://github.com/andresalemn/arduino-bot.git
> cd arduino-bot
> 
> # Option 1: Direct Docker build & run
> docker build -t arduinobot-sim -f docker/Dockerfile --target sim .
> docker run -it --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix arduinobot-sim
> 
> # Option 2: Using Docker Compose
> docker compose -f docker/docker-compose.yml --profile sim up sim
> ```

Gazebo and RViz2 windows should appear on your host display within a minute or so of the container starting.

## What this image does NOT include

- No real hardware / serial support — this is simulation-only.
- No `arduinobot_remote` (Flask/Alexa) — that requires your own credentials and isn't part of the demo.
- No live code editing — the source is compiled into the image at build time. If you make local changes, they won't be reflected until you rebuild the image (or switch to the [dev container](./02-devcontainer-setup.md) workflow).

## Stopping the demo

`Ctrl+C` in the terminal, or close the container's Gazebo/RViz windows. The `--rm` flag in the run command above means the container is removed automatically on exit — no cleanup needed.
