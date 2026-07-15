# AGENTS.md

## Package README Guidelines

Each ROS 2 package should include a `README.md` containing:

- A one- or two-sentence description of the package.
- A brief overview of the package structure.
- The package's responsibilities and scope.
- The nodes it provides and their purpose.
- Whether each node is a regular node or a lifecycle node, including its expected lifecycle behavior, when applicable.
- Topics published and subscribed.
- Services and actions provided or used, when applicable.
- Configuration parameters and their default configuration files.
- Available launch files and their purpose.
- Basic build and run instructions, when applicable.
- Key runtime dependencies and external requirements.

Keep the README focused on the package itself: what it does, how to configure it, and how to use it. Detailed API documentation belongs in Doxygen comments rather than in the README.

## Doxygen Documentation Guidelines

Use Doxygen comments to document the package's public API and implementation details, regardless of whether the package is implemented in C++, Python, or a combination of both.

Document:
- Public modules.
- Public classes.
- Public functions and methods.
- Public interfaces exposed by the package.
- Enums, constants, and other public types when their purpose is not immediately obvious.

Do not use Doxygen comments in:
- `README.md`
- `package.xml`
- `CMakeLists.txt`
- `setup.py`
- `setup.cfg`
- Launch files.
- Configuration files (e.g., YAML).
- Resource files.
- Test files, unless documenting a non-trivial testing utility.

Use regular comments where they improve readability, such as explaining:
- The purpose of a launch file.
- Why nodes are launched in a particular order.
- Non-obvious launch arguments or remappings.
- Configuration choices in YAML files.
- Complex implementation details that are not part of the public API.

Avoid comments that merely restate what the code already expresses. Keep the comments simple, leave complex explanaitions to de READMEs.