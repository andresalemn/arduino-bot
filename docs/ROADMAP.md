# Engineering & RobotOps Roadmap

This document outlines the software engineering, DevOps, and **RobotOps** roadmap for the Arduinobot project. It acts as a guide to transition this repository from an educational workspace into an industry-grade, production-ready robotics codebase.

---

## 🎯 Phase 1: Near-Term Priorities (High Value / Quick Wins)

These features focus on basic code hygiene, automated build verification, and preventing broken commits.

### 1. Code Formatting & Linting (Local + Pre-Commit)
- [ ] **Python Formatting (`black` + `flake8`):** Configure `pyproject.toml` or `setup.cfg` to enforce PEP 8 formatting across all Python nodes (`arduinobot_remote`, `arduinobot_controller`, `arduinobot_py_examples`).
- [ ] **C++ Formatting (`clang-format`):** Add a `.clang-format` rules file matching ROS 2 Google/Open Robotics coding standards.
- [ ] **Pre-commit Hooks (`.pre-commit-config.yaml`):** Install `pre-commit` locally to automatically check whitespace, validate YAML/XML files, and run code formatters before every `git commit`.

### 2. Basic GitHub Actions CI Pipeline (Option A)
Create `.github/workflows/ci.yml` triggered on every Pull Request and push to `main`:
- [ ] **Code Quality Check Job:** Run `black --check`, `flake8`, and `clang-format --dry-run` to ensure incoming code follows style standards.
- [ ] **Headless Colcon Build Job:** Spin up a clean `osrf/ros:humble-desktop-full` container on GitHub runners and execute `colcon build --symlink-install` to guarantee no broken builds enter the main branch.

---

## 🚀 Phase 2: Medium-Term RobotOps (Future Enhancements)

Features to explore as you dive deeper into **RobotOps** and automated container deployment.

### 1. Automated Docker Image Publishing (GHCR)
- [ ] Create `.github/workflows/docker-publish.yml` to automatically build and push the standalone simulation image (`ghcr.io/andresalemn/arduinobot-sim:latest`) to GitHub Container Registry whenever a new release tag is pushed.

### 2. GitHub Repository Templates & Community Hygiene
- [ ] **Issue Templates (`.github/ISSUE_TEMPLATE/`):** Add structured templates for bug reports and feature requests.
- [ ] **Pull Request Template (`.github/PULL_REQUEST_TEMPLATE.md`):** Add a PR checklist ensuring tests and documentation links were updated.

---

## 🔮 Phase 3: Long-Term Vision (Full RobotOps Ecosystem)

Advanced features to keep in mind for future reference or for your next robot project.

### 1. Automated Documentation Generation (`rosdoc2`)
- [ ] Configure `rosdoc2` to parse Doxygen comments in C++ headers (`include/`) and Python docstrings.
- [ ] Build a GitHub Action to deploy generated HTML documentation automatically to **GitHub Pages** (`https://andresalemn.github.io/arduino-bot/`).

### 2. Headless System & Integration Testing (`colcon test`)
- [ ] Write launch tests using `launch_testing` to verify nodes (`task_server`, `alexa_interface`, `slider_control`) launch and exchange ROS 2 topics/actions without throwing unhandled exceptions.
- [ ] Integrate `colcon test` execution into the GitHub Actions CI pipeline.

### 3. Simulation & Fleet Telemetry Logging
- [ ] Explore containerized `rosbag2` recording pipelines for simulation data playback.
- [ ] Implement remote hardware health monitoring / heartbeat diagnostics nodes.

---

## 📌 Sitemap & Related Guides

- 🐳 **[Docker & Dev Container Documentation](./docker/01-overview.md)**
- ⚡ **[Electronics & Hardware Setup](./hardware/electronics.md)**
- 🗣️ **[Alexa Voice Skill Documentation](./alexa/1_introduction.md)**
