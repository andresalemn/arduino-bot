# AGENTS.md

## Repository Conventions

- This repository is licensed under the MIT License.
- Whenever a file requires specifying a software license (e.g., `package.xml`, `setup.py`, `setup.cfg`, `pyproject.toml`, or source file headers when applicable), use `MIT`.
- Do not use or introduce any other license unless explicitly instructed.

---

## Documentation Philosophy

Follow the official ROS 2 documentation guidelines whenever practical. The goal is to produce documentation that is clear, consistent, maintainable, and compatible with the ROS 2 documentation ecosystem.

- Documentation should follow ROS 2 conventions and remain compatible with future integration into tooling such as `rosdoc2`.

- Package documentation, C++ API documentation, and Python API documentation should follow the conventions expected by the ROS 2 community.

---

# Package Documentation Guidelines

## README.md

Each ROS 2 package must include a `README.md` at the package root (alongside `package.xml` and `CMakeLists.txt`).

The README must contain:

- A one- or two-sentence description of the package.
- The package's purpose and responsibilities.
- A brief overview of the package structure.
- The nodes provided by the package and their purpose.
- Whether nodes are regular nodes or lifecycle nodes, when applicable.
- Topics published and subscribed.
- Services and actions provided or used, when applicable.
- Parameters and their default configuration files, when applicable.
- Available launch files and their purpose.
- Basic build and run instructions, when applicable.
- Package dependencies and external requirements, when applicable.

Keep the README focused on the package itself:
- What the package does.
- How to configure it.
- How to build it.
- How to run it.
- How it integrates with other ROS 2 components.

Do not include detailed API documentation in the README. API documentation belongs in the source code and should be generated through documentation tooling.

Write Markdown documentation using syntax compatible with Sphinx/MyST to facilitate future `rosdoc2` integration.

---

# Source Code Documentation Guidelines

Document public APIs using the documentation conventions appropriate for each implementation language.

## C++

Use Doxygen comments to document:

- Public headers.
- Public classes.
- Public functions and methods.
- Public enums, constants, and other public interfaces.
- Non-obvious behavior or constraints of public APIs.

Each public source file should begin with a Doxygen file comment describing:

- The purpose of the file.
- The main classes or functionality provided.
- Any relevant design considerations.

Avoid documenting trivial implementation details.

---

## Python

Use Python docstrings compatible with Sphinx autodoc to document:

- Public modules.
- Public classes.
- Public functions and methods.
- Public ROS 2 interfaces exposed by the package.

Each public Python module should begin with a module-level docstring describing:

- The purpose of the module.
- The primary classes or functionality it provides.

Docstrings should describe:
- Purpose.
- Expected inputs and outputs.
- Important behavior.
- Constraints or assumptions.

Avoid documenting implementation details that are not relevant to users of the API.

---

# Additional Documentation

For packages requiring documentation beyond the README and generated API documentation, use a `doc/` directory.

Examples of content that belongs in `doc/`:

- Tutorials.
- Usage guides.
- Architecture documentation.
- Design documents.
- Detailed configuration guides.
- Integration guides.

Additional documentation should be written in a format compatible with Sphinx to allow future integration with `rosdoc2`.

---

# Package Metadata Guidelines

Keep `package.xml` metadata complete and up to date.

Ensure the package metadata accurately describes:

- Package description.
- Maintainers.
- License information.
- URLs.
- Dependencies.

Package metadata is consumed by ROS tooling and documentation generation systems.

---

# Documentation Exclusions

Do not use API documentation comments in:

- `README.md`
- `package.xml`
- `CMakeLists.txt`
- `setup.py`
- `setup.cfg`
- Launch files.
- Configuration files (e.g., YAML, XML, SDF, URDF, Xacro)
- Resource files.
- Test files, unless documenting a non-trivial testing utility.

---

# Regular Comments

Use regular comments where they improve readability or explain design decisions.

Examples:

## Launch files

Explain:

- The purpose of the launch file.
- Why nodes are launched in a particular order.
- Non-obvious launch arguments.
- Remappings.
- Lifecycle transition behavior.

## Build files

Explain:

- Non-obvious `CMakeLists.txt` logic.
- Build options.
- Plugin registration.
- Installation decisions.

## Configuration files

Explain:

- Parameter choices.
- Hardware assumptions.
- Non-obvious configuration values.

## XML-based files

Use XML comments to document non-obvious configuration choices in XML-based files such as:

- SDF world files.
- URDF robot descriptions.
- Xacro macros.
- Other XML-based ROS resources.

Follow XML syntax rules when writing comments:

- Comments must start with `<!--` and end with `-->`.
- The sequence `--` is not allowed inside comments.
- Avoid using dashed separators inside comments, as they can create invalid XML.

Prefer separators that do not contain consecutive hyphens:

```xml
<!--
  Physics Configuration
  =====================
-->
```

## Source code

Explain:

- Complex algorithms.
- Important design decisions.
- Workarounds.
- Constraints that are not obvious from the implementation.

Avoid comments that merely restate what the code already expresses.

---

# General Documentation Principles

- Document **what** a component does and **why** it exists.
- Prefer documenting behavior, interfaces, and assumptions over implementation details.
- Keep package documentation focused on ROS concepts:
  - Nodes.
  - Topics.
  - Services.
  - Actions.
  - Parameters.
  - Lifecycle behavior.
  - Launch files.
  - Dependencies.

- Keep API documentation focused on public interfaces.
- Keep file-level documentation concise and focused on the role of the file.
- Ensure documentation remains accurate as the implementation evolves.
- Prefer documentation that is compatible with official ROS 2 tooling and future `rosdoc2` adoption.