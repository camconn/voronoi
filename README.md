# voronoi

A Simple program to create [Voronoi diagrams](https://en.wikipedia.org/wiki/Voronoi_diagram).

![Voronoi Diagram](examples/voronoi.png?raw=true "Voronoi Diagram")

### Usage

```
make
./voronoi
```
And then an image will be created called `voronoi.png`.

For more detailed usage, run the command `./voronoi -h`.

### Creating themes

Creating your own theme is really simple. Just make a theme name enclosed in `[brackets]` and
list the theme's full-length (6 character) hex color codes behind it. For example, to make a
red, white, and blue theme called *patriot*:

```
[patriot]
FFFFFF
FF0000
0000FF
```

### Examples

A few samples of output are available in the `examples` subdirectory.

### License

This project is licensed under the GNU Public License, Verion 3 or Later. A copy of this license may be
found in the file `LICENSE`.
