## Build

To build, simply enter `premake4 gmake` in the folder containing `premake4`.
Each time you add or remove a file in the sources, you should rebuild the project using this command.

## Make

There are several configuration for the makefile:
 * `make config=debug64` (default use when enter `make`)
 * `make config=debug32` (not working)
 * `make config=release64` (comile using maximum acceleration)
 * `make config=release32` (not working)

## Bug report
 * mauvais calcul des min_p.z et max_p.z qulque part, se vois lorsqu'on exporte les texture en utilisant min_p et max_p pour normaliser

## TODO
 * erosion thermique
 * erosion techtonique (grosse formule du cours)
 * eclairage direct
 * chemins
 * vegetation

## What is done
 * TODO