# libtcx

A C TCX file parser.

This parser consumes [TCX files](https://en.wikipedia.org/wiki/Training_Center_XML). It builds a set of nested, singly linked lists that correspond to the structure of a TCX file. It also aggregates the data and performs some basic statistical calculations, such as:

- Split/lap:

  - Speed average/maximum/minimum
  - Heart rate average/maximum/minimum
  - Cadence average/maximum/minimum
  - Maximum and minimum elevation
  - Total elevation gain / loss
  - Grade adjusted time (in seconds)

- Activity:
  - Speed average/maximum/minimum
  - Heart rate average/maximum/minimum
  - Cadence average/maximum/minimum
  - Maximum and minimum elevation
  - Total elevation gain / loss

Each trackpoint contains latitude/longitude coordinates, if GPS data is present. It tracks the activities starting and ending points, total time, total distance, etc... You can get a better understanding of the various bits of data that can be extracted by looking at the data structures in `tcx.h`.

## Development

### Notes

#### libxml2

On Mac, libxml2 needs to be installed and needs to find it's way in to the include path. Traditional methods failed me and what I ended up with was symlinking the Homebrew install path to the include path. Not great, but it works.

```sh
sudo ln -s /usr/local/opt/libxml2/include/libxml2/libxml /usr/local/include/libxml
```

#### Valgrind

```sh
valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --track-origins=yes ./libtcx some-tcx-file.tcx
```
