# Docker in Arduinobot — Overview

This repo uses Docker for two distinct purposes. They are related but not the same thing, and it's worth understanding the split before touching either one.

## The two use cases

### 1. Development environment (any contributor)

A **VS Code Dev Container** that provides a clean, reproducible ROS 2 Humble + Gazebo + MoveIt 2 environment without installing anything permanently on the host machine. This is the container used for actual day-to-day development: editing code, building with `colcon`, running simulation, debugging.

- Repo source is bind-mounted into the container — edits on disk are instantly visible inside the container, no image rebuild required for code changes.
- Used constantly, on whichever machine I'm working from (native Ubuntu dual-boot, or Windows via Docker Desktop + WSL2).
- Not meant to be a polished "product" — it's scaffolding for my own iteration speed and for anyone who wants to contribute code.

See [`02-devcontainer-setup.md`](./02-devcontainer-setup.md).

### 2. Simulation demo (for curious external developers)

A **standalone, pre-built image** aimed at someone who finds this repo, doesn't own the physical arm, and doesn't want to install ROS 2 + Gazebo just to see what it does. `docker run` it, a Gazebo window comes up, the arm moves through a canned simulation/demo.

- Code is baked into the image at build time (no bind mount, no live editing).
- Meant to be run once or twice to "kick the tires," not to develop against.
- Not expected to scale to many users — this repo isn't widely distributed — but it's good practice, and it's a genuinely nice zero-friction way for someone to evaluate the project.

See [`03-simulation-demo.md`](./03-simulation-demo.md).

## What's explicitly out of scope

Docker does **not** cover:

- **Real hardware / serial connection to the physical Arduino.** This requires direct USB device access, physical wiring, and per-machine port configuration. It's documented as a native (host-only) setup path, not containerized.
- **`arduinobot_remote` (Flask + Alexa voice control).** This depends on external credentials (ngrok tunnel, Amazon ASK-SDK skill registration) that don't make sense to bake into a shareable image. It's set up natively per the existing package docs.

If you're here to build the physical robot or wire up voice control, these docs aren't for you — see the respective package READMEs instead.

## Design rationale (the short version)

| | Dev Container | Simulation Demo Image |
|---|---|---|
| Audience | Me / contributors | Curious strangers |
| Code | Bind-mounted (live) | Baked in at build time |
| Rebuild on code change? | No | Yes |
| Frequency of use | Constant | Once or twice |
| GUI (Gazebo/RViz) | Yes | Yes |
| Hardware/serial | No (host-only) | No |

Both images share a common base layer (ROS 2 Humble + Gazebo + MoveIt 2 system dependencies) to avoid duplicating that setup work — they differ mainly in whether source code is copied in and compiled at build time, or mounted and built on demand.

## Prerequisites for either path

- [Docker](https://docs.docker.com/get-docker/) (Docker Desktop on Windows/macOS, Docker Engine on Linux)
- On Windows: WSL2 is required under the hood by Docker Desktop, and **WSLg** (Windows 11, or updated Windows 10) is what makes GUI apps like Gazebo/RViz work without extra X server setup. See [`04-troubleshooting.md`](./04-troubleshooting.md) for details and older-Windows fallbacks.

## Next steps

- Setting up and using the dev environment day-to-day → [`02-devcontainer-setup.md`](./02-devcontainer-setup.md)
- Trying the simulation demo as an outsider → [`03-simulation-demo.md`](./03-simulation-demo.md)
- Something broke → [`04-troubleshooting.md`](./04-troubleshooting.md)
