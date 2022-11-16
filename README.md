## Build

To build, simply enter `premake4 gmake` in the folder containing `premake4`.
Each time you add or remove a file in the sources, you should rebuild the project using this command.

## Make

There are several configuration for the makefile:
 * `make config=debug64` (default use when enter `make`)
 * `make config=debug32` (not working)
 * `make config=release64` (comile using maximum acceleration)
 * `make config=release64` (not working)