// The sherpa_41 browser engine, licensed under MIT. (c) hafiz, 2019

#include <Magick++.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include "layout.h"
#include "parser/args.h"
#include "parser/css.h"
#include "parser/html.h"
#include "renderer/canvas.h"
#include "style.h"
#include "visitor/printer.h"

auto inline odefault(const std::string& option) -> const std::string& {
  static std::unordered_map<std::string, std::string> defaults{
      {"--html", "examples/sherpa-webpage.html"},
      {"--css", "examples/sherpa-webpage.css"},
      {"--width", "2880"},
      {"--height", "1620"},
      {"--out", "output.png"},
  };
  return defaults[option];
}

auto help() -> std::string {
  auto deftext = [](const std::string& option) {
    return "(Default: " + odefault(option) + ")\n";
  };

  std::stringstream help;
  help << "USAGE: sherpa_41 [options]\n";
  help << "\n";
  help << "OPTIONS:\n";
  help << "        --html <file>             HTML file to parse " << deftext("--html");
  help << "        --css <file>              CSS file to parse " << deftext("--css");
  help << "        -W, --width <size>        Browser width, in pixels "
       << deftext("--width");
  help << "        -H, --height <size>       Browser height, in pixels "
       << deftext("--height");
  help << "        -o, --out <file>          Output file " << deftext("--out");
  help << "        -h, --help                Show this help screen";

  return help.str();
}

/**
 * Gets an argument from the CLI
 * @param Long long arg (--arg).
 * @param Short short arg (-a). Empty string if no short arg.
 * @return value of argument
 */
auto getArg(const std::string& Long, const std::string& Short = "") -> const std::string& {
  ArgsParser& args = ArgsParser::instance();
  try {
    if (args.cmdOptionExists(Long)) {
      return args.getCmdOption(Long);
    } else if (!Short.empty() && args.cmdOptionExists(Short)) {
      return args.getCmdOption(Short);
    }

    auto& Default{odefault(Long)};
    if (!Default.empty()) {
      return Default;
    }

    throw std::invalid_argument("Required option " + Long + " not found");
  } catch (const std::invalid_argument& exc) {
    // Argument specified but value is not.
    std::cout << "ERROR: " << exc.what() << "\n\n";
    std::cout << help() << "\n";
    exit(1);
  }
}

auto main(int argc, char** argv) -> int {
  auto& args = ArgsParser::instance(argc, argv);

  if (args.cmdOptionExists("-h") || args.cmdOptionExists("--help")) {
    std::cout << "A trivial browser engine.\n\n";
    std::cout << help() << "\n";
    return 0;
  }

  std::ifstream fhtml{getArg("--html")};
  std::ifstream fcss{getArg("--css")};
  std::string output{getArg("--out", "-o")};
  float width{std::stof(getArg("--width", "-W"))};
  float height{std::stof(getArg("--height", "-H"))};

  std::stringstream buffer;
  buffer << fhtml.rdbuf();
  HTMLParser htmlParser(buffer.str());

  buffer.clear();
  buffer.str(std::string());
  buffer << fcss.rdbuf();
  CSSParser cssParser(buffer.str());

  Layout::Rectangle frame(0., 0., width, height);

  auto dom = htmlParser.evaluate();
  auto stylesheet = cssParser.evaluate();
  auto styledDom = Style::StyledNode::from(dom, stylesheet);
  auto paintLayout = Layout::Box::from(styledDom, Layout::BoxDimensions(frame));

  Magick::InitializeMagick(*argv);

  Canvas canvas(frame, paintLayout);

  Magick::Image im;
  im.read(static_cast<uint64_t>(width), static_cast<uint64_t>(height), "RGBA",
          Magick::CharPixel, canvas.getPixels().data());

  im.write(output);

  std::cout << "Output written to " << output << ".\n";
}
