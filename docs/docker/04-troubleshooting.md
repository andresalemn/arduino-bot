# Docker Troubleshooting

Common issues when working with the dev container or simulation demo, and how to resolve them. This is a living doc — add to it as new issues come up.

---

## `colcon build` errors after switching between host and container builds

**Symptom:** Confusing CMake or linker errors, missing symbols, or "package not found" errors that appear out of nowhere after previously building fine.

**Cause:** `build/`, `install/`, and `log/` in `arduinobot_ws/` are written to the bind-mounted host filesystem. If the same workspace checkout is built both inside the dev container and natively on the host (outside Docker), CMake's cached absolute paths and toolchain versions can conflict.

**Fix:**
```bash
cd arduinobot_ws
rm -rf build install log
colcon build --symlink-install
```
Then commit to building from **one environment only** for that checkout going forward (see [`02-devcontainer-setup.md`](./02-devcontainer-setup.md#important-pick-one-build-path-per-machine)).

---

## GUI apps on Windows

**Symptom:** Gazebo/RViz2 container starts, no crash, but no window ever appears.

**First check — Windows version:** WSLg (which enables GUI passthrough automatically) requires **Windows 11**, or **Windows 10** with recent updates and WSL2 installed via `wsl --update`. Run `wsl --version` in PowerShell to confirm WSLg is present (it lists a WSLg version if so).

**If WSLg is present but windows still don't appear:**
- Confirm Docker Desktop is set to use the **WSL2 backend** (Settings → General → "Use the WSL 2 based engine").
- Restart Docker Desktop after any WSL update.
- Try a minimal GUI test first to isolate the issue from ROS2/Gazebo specifics:
  ```bash
  docker run -it --rm ghcr.io/ros2/ros:humble ros2 run rqt_graph rqt_graph
  ```

**If on older Windows (no WSLg) or macOS:**
- You need a standalone X server:
  - Windows: [VcXsrv](https://sourceforge.net/projects/vcxsrv/), launched with "Disable access control" checked.
  - macOS: [XQuartz](https://www.xquartz.org/), with "Allow connections from network clients" enabled in preferences.
- Set `DISPLAY` to point at the host's IP (not `localhost`, since the container is a separate network namespace from a real X server running outside WSL/Docker):
  ```bash
  docker run -it --rm -e DISPLAY=host.docker.internal:0 ...
  ```
- This path is noticeably clunkier than WSLg — if you're using this repo regularly on Windows, upgrading to Windows 11 (or enabling WSLg on Windows 10) is worth it.

---

## Gazebo renders slowly / no GPU acceleration in WSL2

**Symptom:** Gazebo window appears but rendering is choppy or clearly software-rendered.

**Cause:** WSLg needs a WSL-compatible GPU driver installed on the Windows host to pass through hardware acceleration.

**Fix:** Install the appropriate driver for your GPU vendor:
- NVIDIA: [CUDA on WSL driver](https://developer.nvidia.com/cuda/wsl)
- AMD / Intel: check for "WSL" or "WSL2" support in your latest driver package — both vendors ship WSLg-compatible drivers in current releases.

Without a compatible driver, Gazebo will still run, just without GPU acceleration — fine for basic testing, not ideal for extended simulation work.

---

## `postCreateCommand` build fails on first container start

**Symptom:** Container builds and starts, but the automatic `colcon build --symlink-install` fails immediately.

**Common causes:**
- A new package was added to `arduinobot_ws/src/` with a dependency not yet in the Dockerfile's `apt install` list (rosdep-managed deps need to be added to the base image, not just `package.xml`).
- Stale `build/`/`install/` left over from a host build (see the first section above) — delete and retry.

**Fix:** Run manually inside the container terminal to see the full error:
```bash
cd arduinobot_ws
rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install
```

---

## Container can't see my real Arduino / serial port

This is expected — neither the dev container nor the simulation image pass through USB/serial devices. Real hardware work is intentionally done natively on the host. See [`01-overview.md`](./01-overview.md#whats-explicitly-out-of-scope) for why, and the (forthcoming) hardware setup docs for the native workflow.

---

## Something else broke

Open an issue with the exact command run, the full error output, and your OS + Docker version (`docker version`). If it's a recurring issue, it belongs in this file — add a section above following the same Symptom / Cause / Fix format.
