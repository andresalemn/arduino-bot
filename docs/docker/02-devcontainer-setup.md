# Dev Container Setup & Workflow

This is the environment used for actual development on Arduinobot: editing code, building with `colcon`, and running simulation, all inside a reproducible ROS 2 Humble container.

> If you just want to *see* the robot move without setting up a dev environment, use the [simulation demo](./03-simulation-demo.md) instead — it's a lighter-weight path with no live editing.

## Prerequisites

- [Docker](https://docs.docker.com/get-docker/) installed and running
- [VS Code](https://code.visualstudio.com/) with the [Dev Containers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)
- **On Windows:** Docker Desktop with the WSL2 backend enabled (this is the default in recent installs). GUI apps (Gazebo, RViz2) work out of the box via **WSLg** on Windows 11 / updated Windows 10 — no manual X server setup needed. See [troubleshooting](./04-troubleshooting.md#gui-apps-on-windows) if windows don't appear.
- **On native Ubuntu:** X11 forwarding is handled by the devcontainer config directly; no extra setup beyond Docker itself.

## Opening the project

1. Clone the repo and open the folder in VS Code.
2. VS Code should detect `.devcontainer/devcontainer.json` and prompt **"Reopen in Container."** Accept it.
   - If it doesn't prompt automatically: open the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`) → **Dev Containers: Reopen in Container**.
3. First launch will build the base image (ROS 2 Humble + Gazebo + MoveIt 2 + build tools) — this takes a while the first time, then is cached.
4. Once inside, the `postCreateCommand` automatically runs an initial `colcon build --symlink-install` on `arduinobot_ws/` so the workspace is ready to use immediately.

## Day-to-day workflow

- Edit code in VS Code as normal — you're editing the same files whether "inside" or "outside" the container, since the repo is bind-mounted. There's no separate copy to sync.
- IntelliSense, the C++ language server, and debugging all run **inside** the container, so they see the actual ROS 2 headers and dependencies — no more mismatched include paths against a host install.
- Terminal opened in VS Code (or attached via WezTerm/CLI) automatically sources both ROS 2 Humble (`/opt/ros/humble/setup.bash`) and the built workspace (`arduinobot_ws/install/setup.bash`) via `~/.bashrc`. Simply run:
  ```bash
  cd arduinobot_ws
  colcon build --symlink-install
  ros2 launch arduinobot_bringup simulated_robot.launch.py
  ```
- `--symlink-install` means editing Python nodes, launch files, or config/YAML doesn't require a rebuild — only changes to C++ source need `colcon build` again.

## Important: pick one build path per machine

`colcon build` writes `build/`, `install/`, and `log/` directly into `arduinobot_ws/` on disk — since the repo is bind-mounted, these are real files on your host filesystem, not container-internal state.

**Don't build the same workspace checkout both inside the container and natively on the host.** CMake caches absolute paths and toolchain details; mixing container-built and host-built artifacts in the same `build/`/`install/` folders causes confusing linker/CMake errors.

**Rule of thumb:** if you're working inside the dev container, always build inside it. If you occasionally need a native host build (e.g. to test against real hardware without Docker), delete `build/`, `install/`, and `log/` first, or use a separate workspace clone entirely.

These folders are already gitignored, so this is purely a local-workspace-hygiene issue — it won't affect what gets committed or what a fresh clone looks like.

## Alternative entry point: terminal-only access (no VS Code window)

"Reopen in Container" is the easiest path, but it's not the only one — and you don't need VS Code open at all to get a shell into the same environment. The devcontainer is a regular Docker container underneath; anything capable of exec'ing into a container can attach to it, including your own terminal emulator (WezTerm, iTerm, plain tmux, etc.) with your own tmux config, entirely independent of VS Code's integrated terminal.

The cleanest way to do this is the official **Dev Containers CLI**, the same engine VS Code uses internally:

```bash
npm install -g @devcontainers/cli
```

From the repo root, in any terminal:

```bash
# Build/start the container defined by .devcontainer/devcontainer.json (idempotent — safe to re-run)
devcontainer up --workspace-folder .

# Get a shell inside it
devcontainer exec --workspace-folder . bash
```

This gives you the exact same environment VS Code would attach to, without needing VS Code running at all — useful for scripting, CI, SSH sessions, or simply preferring your own terminal setup over the integrated one. A typical flow looks like: open the project in VS Code once to get IntelliSense/debugging when you want it, and separately open a shell into the same running container from your own terminal + multiplexer for actual command-line work.

> **Note:** if the container isn't already running (e.g. VS Code hasn't opened it yet), `devcontainer up` builds and starts it fresh — no need to open VS Code first just to bootstrap the environment.
>
> If VS Code *has* already started the container for this repo, `devcontainer up` detects and reuses that same running instance rather than creating a second one (it keys off the workspace folder path) — so this same command works whether VS Code opened it first or not.

### Attaching from another terminal to a container VS Code already started

A common variation on the above: open the repo in VS Code as usual ("Reopen in Container"), then later attach to *that same running container* from WezTerm (or any terminal) — no separate `devcontainer up` needed, since it's already running.

Find the container:

```bash
docker ps --filter "label=devcontainer.local_folder=$(pwd)"
```

That label is set by the VS Code extension itself, so this filters to exactly the container it has open for this repo (handy if you have several devcontainers running across different projects). Then attach a second shell to it:

```bash
docker exec -it arduinobot-dev bash
```

Or using subshell label filtering:

```bash
docker exec -it $(docker ps -q --filter "label=devcontainer.local_folder=$(pwd)") bash
```

This is the exact same running container VS Code is using — same filesystem, same live processes. If `simulated_robot.launch.py` is running in the VS Code integrated terminal, `ros2 node list` from the WezTerm shell will see it too. Start tmux here for the pane-per-process workflow described below; it keeps running independently of whether VS Code's window is still open.

**In short:** `devcontainer up`/`exec` (previous section) is the VS Code-independent path — it works whether or not VS Code has ever touched this repo. `docker ps` + `docker exec` (this section) is for attaching to a container VS Code already started. Either gets you to the same place; use whichever fits how you're starting the session that day.

### Why tmux is worth it for ROS 2 work specifically

ROS 2 development tends to need several long-running processes visible at once — a launch file, `ros2 topic echo` on something you're debugging, `rqt_graph`, a build/test loop, maybe a `ros2 bag record` — and juggling these across separate terminal tabs gets tedious fast, especially if you're also switching between host and container contexts. A multiplexer session with dedicated panes/windows per process (one for `colcon build --symlink-install`, one for the launch file's log output, one for ad-hoc `ros2 topic`/`ros2 node` inspection, one for a bag replay) lets you see everything at a glance and survives you disconnecting — genuinely useful once a project has more moving parts than `arduinobot`'s current 4-DOF sim, and a good habit to build now.

This repo intentionally doesn't ship or assume any particular tmux configuration — that's a personal dev-environment choice, not something specific to Arduinobot. If you want tmux available inside the container itself (so a session persists even after closing your terminal), install it in your own Dockerfile customization and bind-mount your own `.tmux.conf` via `devcontainer.json`'s `mounts` — kept outside this repo, in your personal dotfiles.

## Real hardware note

The dev container does **not** currently pass through USB/serial devices. If you're connecting a real Arduino over serial, build and run `arduinobot_firmware`/`arduinobot_bringup real_robot.launch.py` natively on the host, not inside this container. This is intentional — see [`01-overview.md`](./01-overview.md#whats-explicitly-out-of-scope).

## Stopping / rebuilding the container

- Close VS Code or use **Dev Containers: Reopen Folder Locally** to exit the container — your `build/`/`install/` artifacts on disk are untouched and will still be there next time.
- If you change the Dockerfile or `devcontainer.json` (e.g. adding a new apt dependency), use **Dev Containers: Rebuild Container** to pick up the change. This rebuilds the image layer only — it does not touch your workspace source or existing build artifacts.
