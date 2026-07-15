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