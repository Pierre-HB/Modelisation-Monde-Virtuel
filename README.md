## Build

To build, simply enter `premake4 gmake` in the folder containing `premake4`.
Each time you add or remove a file in the sources, you should rebuild the project using this command.

## Make

There are several configuration for the makefile:
 * `make config=debug64` (default use when enter `make`)
 * `make config=debug32` (not working)
 * `make config=release64` (comile using maximum acceleration)
 * `make config=release32` (not working)

## Report

 * Parler du calcul des prob pour les arbres!

## Bug report
 * Nothing

## TODO
 * avoid tree in houses
 * avoid houses on road connecting cities
 * make vegetation/houses/city_circle/... appera and disapear by pressing a button


## What is done
 * surcharge des operateur +, *, etc pour les scalar field
 * surcharge des operateur +, *, etc pour les infinit texture
 * convolution et map des scalar field
 * export d'un scalar field sous forme d'image
 * ray-tracing sur un terrain et calcul de l'eclairage ambiant
 * bruit value et Perlin 2D
 * Pathing
