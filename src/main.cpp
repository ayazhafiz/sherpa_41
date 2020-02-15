// The sherpa_41 browser engine, licensed under MIT. (c) hafiz, 2019

#include <Magick++.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "layout.h"
#include "parser/args.h"
#include "parser/css.h"
#include "parser/html.h"
#include "renderer/canvas.h"
#include "style.h"
#include "visitor/printer.h"

/**
 * Prints usage menu
 */
void printHelp() {
  static std::string helpText = R"(
A trivial browser engine.

USAGE: sherpa_41 [options]

OPTIONS:
	--html <file>             HTML file to parse (Default: examples/sherpa-webpage.html)
	--css <file>              CSS file to parse (Default: examples/sherpa-webpage.css)
	-W, --width <size>        Browser width, in pixels (Default: 2880)
	-H, --height <size>       Browser height, in pixels (Default: 1620)
	-o, --out <file>          Output file (Default: output.png)
	-h, --help                Show this help screen
)";
  std::cout << helpText << "\n";
}

/**
 * Gets input files from CLI args
 * @param html html file
 * @param css css file
 */
void getInFiles(std::ifstream& html, std::ifstream& css) {
  ArgsParser& args = ArgsParser::instance();
  auto htmlFileName = args.getCmdOption("--html");
  auto cssFileName = args.getCmdOption("--css");
  if (!htmlFileName.empty()) {
    html = std::ifstream(htmlFileName);
  }
  if (!cssFileName.empty()) {
    css = std::ifstream(cssFileName);
  }
}

/**
 * Gets an argument from the CLI
 * @param shrt short arg (-a). Empty string if no short arg.
 * @param lng long arg (--arg). Empty string if no long arg.
 * @param deflt default value to use. Empty string if no default value.
 * @return value of argument
 */
auto getArg(const std::string& shrt = "",
            const std::string& lng = "",
            const std::string& deflt = "") -> const std::string& {
  ArgsParser& args = ArgsParser::instance();
  try {
    if (!shrt.empty() && args.cmdOptionExists(shrt)) {
      return args.getCmdOption(shrt);
    } else if (!lng.empty() && args.cmdOptionExists(lng)) {
      return args.getCmdOption(lng);
    }
  } catch (const std::invalid_argument& exc) {
    // argument specified but value not, exit on error.
    std::cout << "ERROR: " << exc.what() << "\n";
    printHelp();
    exit(1);
  }
  return deflt;
}

auto main(int argc, char** argv) -> int {
  ArgsParser& args = ArgsParser::instance(argc, argv);

  if (args.cmdOptionExists("-h") || args.cmdOptionExists("--help")) {
    std::cout << "A trivial browser engine.\n";
    printHelp();
    return 0;
  }

  std::ifstream htmlFile(getArg("", "--html", "examples/sherpa-webpage.html"));
  std::ifstream cssFile(getArg("", "--css", "examples/sherpa-webpage.css"));
  std::string output(getArg("-o", "--out", "output.png"));

  std::stringstream buffer;
  buffer << htmlFile.rdbuf();
  HTMLParser htmlParser(buffer.str());

  buffer.clear();
  buffer.str(std::string());
  buffer << cssFile.rdbuf();
  CSSParser cssParser(buffer.str());

  Layout::Rectangle frame(0., 0., std::stof(getArg("-W", "--width", "2880")),
                          std::stof(getArg("-H", "--height", "1620")));

  auto dom = htmlParser.evaluate();
  auto stylesheet = cssParser.evaluate();
  auto styledDom = Style::StyledNode::from(dom, stylesheet);
  auto paintLayout = Layout::Box::from(styledDom, Layout::BoxDimensions(frame));

  Magick::InitializeMagick(*argv);

  Canvas canvas(frame, paintLayout);

  Magick::Image im;
  im.read(static_cast<uint64_t>(frame.width), static_cast<uint64_t>(frame.height), "RGBA",
          Magick::CharPixel, canvas.getPixels().data());

  // Write the image to output
  im.write(output);

  std::cout << "Output written to " << output << ".\n";
}
