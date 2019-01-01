<h3 name="sherpa-header" id="sherpa-header" align="center">
  <br />
	<img width="480" src="./examples/sherpa-webpage-output.png" alt="sherpa_41 test webpage" />
  <span width="400" />
	<img width="75" src="./examples/blank.png" alt="sherpa_41 test webpage" />
	<img width="280" src="./examples/test-output.png" alt="sherpa_41 test webpage" />
  <h5 align="center"><sub>A <code>sherpa_41</code>-rendered webpages</sub></h5>
</h3>

# `sherpa_41` :mountain:

[![Build Status](https://travis-ci.com/ayazhafiz/sherpa_41.svg?branch=master)](https://travis-ci.com/ayazhafiz/sherpa_41)
[![Coverage Status](https://coveralls.io/repos/github/ayazhafiz/sherpa_41/badge.svg?branch=master)](https://coveralls.io/github/ayazhafiz/sherpa_41?branch=master)

`sherpa_41` is a toy browser engine, built loosely from [Limpet's _Let's build a browser engine!_](https://limpet.net/mbrubeck/2014/08/08/toy-layout-engine-1.html)
with architectures introduced in [CS3251](https://vuse-cs3251.github.io).

This app makes use of modern `C++` and established design patterns to create
an engine that is easy to understand and extend. Note, however, that the
engine is ___not___ designed to be robust, or even useful.

### Usage

```bash
USAGE: sherpa_41 [options]

OPTIONS:
        --html                  HTML file to parse (=assets/rainbowBox.html)
        --css                   CSS file to parse (=assets/rainboxBox.css)
        -w, --width             Browser width (=800)
        -h, --height            Browser height (=600)
        -o, --out               Output file (=output.png)
        --help                  Show this help screen
```

All option flags have a default if you do not specify them. For the output file,
it is recommended to only use `.png`, `.jpg`, or `.bmp` extensions.

An example of a custom invocation:

```bash
sherpa_41 --html index.html --css style.css -w 1800 -h 1200 -o myIndex.jpg
```

### Building

First, you will need [CMake](https://cmake.org) and [Magick++](https://imagemagick.org/Magick++/).
Both come pre-installed with most Linux distros, and are easy to install on MacOS and Windows.

Clone the repo and its dependencies, then make the project:

```bash
git clone --recursive https://github.com/ayazhafiz/sherpa_41.git
cd sherpa_41 && cmake .
make
./sherpa_41-test && ./sherpa_41
```

### Features

- The HTML parser currently supports elements, comments, and text nodes.

- The CSS parser supports tag, class, id, and wildcard selectors, and has support for text, color (RGB/A, #HEX), and numerical unit declarations.

- The Display module can issue commands to render only rectangular block nodes at this time.

- Sherpa's `Canvas` renderer can currently generate fairly non-trivial webpages like [the header](#sherpa-header).

### Notes

- JavaScript is turned off by default, and can't be turned on. You may say JavaScript is unsupported. I say you're right.

- Please don't try to break the application, because it will. Pretty easily. There is \~just enough\~ error checking to recover trivial mistakes, but auto-closing tags and the like is not supported.

- There is no networking. I.e. you can only browse websites you make... and very limited ones, at that.

##### etymology

idk, i got a sherpa jacket and thought it sounded ambiguous enough to be a spectacular project name.
