<h3 name="sherpa-header" id="sherpa-header" align="center">
  <br />
	<img width="480" src="./examples/sherpa-webpage-output.png" alt="sherpa_41 test webpage" />
  <span width="400" />
	<img width="75" src="./examples/blank.png" alt="sherpa_41 test webpage" />
	<img width="280" src="./examples/test-output.png" alt="sherpa_41 test webpage" />
  <h5 align="center"><sub><code>sherpa_41</code>-rendered webpages</sub></h5>
</h3>

# `sherpa_41` :mountain:

[![Build Status](https://img.shields.io/travis/com/ayazhafiz/sherpa_41/master.svg)](https://travis-ci.com/ayazhafiz/sherpa_41)
[![Build status](https://ci.appveyor.com/api/projects/status/kr7k3fc7o1iwpc97?svg=true)](https://ci.appveyor.com/project/ayazhafiz/sherpa-41)
[![Coverage Status](https://img.shields.io/coveralls/github/ayazhafiz/sherpa_41/master.svg)](https://coveralls.io/github/ayazhafiz/sherpa_41?branch=master)

`sherpa_41` is a toy browser engine, based somewhat on
[Limpet's _Let's build a browser engine!_](https://limpet.net/mbrubeck/2014/08/08/toy-layout-engine-1.html)
with architectures introduced in [CS3251](https://vuse-cs3251.github.io).

This app is meant to be explorative in nature, and is **_not_** designed to be
especially robust, or even useful.

### Usage

```bash
USAGE: sherpa_41 [options]

OPTIONS:
        --html <file>             HTML file to parse (Default: examples/sherpa-webpage.html)
        --css <file>              CSS file to parse (Default: examples/sherpa-webpage.css)
        -W, --width <size>        Browser width, in pixels (Default: 2880)
        -H, --height <size>       Browser height, in pixels (Default: 1620)
        -o, --out <file>          Output file (Default: output.png)
        -h, --help                Show this help screen
```

Almost any file type designed to show images can be used as the output file.
`.png`, `.pdf`, `.jpg` are all great options, and you can even output to a
`.html` file if you want.

An example of a custom invocation:

```bash
sherpa_41 --html index.html --css style.css -W 1800 -H 1200 -o myIndex.jpg
```

### Building

First, you will need [CMake](https://cmake.org) and
[Magick++](https://imagemagick.org/Magick++/).

Clone the repo and its dependencies, then make the project:

```bash
git clone --recursive https://github.com/ayazhafiz/sherpa_41.git
cd sherpa_41 && cmake .
make
./sherpa_41-test && ./sherpa_41
```

A summary of [development notes](#development-notes) is lower on this document.

### Features

- The HTML parser currently supports elements, comments, and text nodes.

- The CSS parser currently supports tag, class, id, and wildcard selectors, and
  has support for text, color (RGB/A, #HEX), and numerical unit declarations.

- The Display module can currently issue commands to render rectangular block
  nodes.

- Sherpa's [Canvas](./src/renderer/canvas.h) renderer can currently generate
  basic webpages like [the header](#sherpa-header).

### Notes

- There is no GUI renderer (yet?!). Until one is made, the front-end is really
  just an image. But a cool, nicely-rendered image, because sherpa has awesome
  renderers.

- JavaScript is turned off by default, and can't be turned on. You may say
  JavaScript is unsupported. I say you're right.

- If you patch this project, I'd love to see what you do. An
  [issue](https://github.com/ayazhafiz/sherpa_41/issues) or
  [pull request](https://github.com/ayazhafiz/sherpa_41/compare) is the easiest
  channel for communication.

### Development Notes

##### Adding features

When adding a new feature or module to `sherpa_41`, please also add appropriate
[GTests](https://github.com/google/googletest) for it in `tests/`. Generally,
the structure of the `tests/` directory should mirror that of `src/`.

Before submitting a [PR](https://github.com/ayazhafiz/sherpa_41/compare), please
take care that your changes pass on
[Travis](https://travis-ci.com/ayazhafiz/sherpa_41).

The Travis build will fail if the codebase is not formatted to the
`.clang-format` spec provided. To format the code, run something like
`clang-format -i {src,tests}/**/*.(h|cpp)`.

##### A Brief Overview of `sherpa_41`'s Design

In order of importance, Sherpa's main design goals are _sound architecture_,
_memory and type safety_, and _efficiency_.

1. **Sound architecture** - because `sherpa_41` is meant to be easy to explore
   and extend, reusable components and solid design patterns are valued
   overwhelmingly above succinctness and efficiency (of course, these aren't
   mutually exclusive).

2. **Memory and type safety** - `sherpa_41`'s memory philosophy is Rust's in a
   C++ wrapper. RAII, smart pointers, and move semantics are... the move. Here
   may be where C++ idioms stray the most, with sherpa using static methods like
   `::from` to return unique pointers to polymorphic objects.

3. **Efficiency** - `sherpa_41` has no excuse not to go fast. This mostly means
   avoiding object transmutation. The performance of renderers could be improved
   significantly.

##### etymology

idk, i got a sherpa jacket and thought the word sounded ambiguous enough to be a
spectacular project name.
