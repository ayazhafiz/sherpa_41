// The sherpa_41 browser engine, licensed under MIT. (c) hafiz, 2019

#include "layout.hpp"
#include "style.hpp"

#include "parser/args.hpp"
#include "parser/css.hpp"
#include "parser/html.hpp"
#include "renderer/canvas.hpp"
#include "visitor/printer.hpp"

#include <Magick++.h>
#include <fstream>
#include <iostream>
#include <sstream>

/**
 * Prints usage menu
 */
void printUsage() {
    static std::string usageText = R"(
USAGE: sherpa_41 [options]

OPTIONS:
	--html <file>             HTML file to parse (Default: examples/sherpa-webpage.html)
	--css <file>              CSS file to parse (Default: examples/sherpa-webpage.css)
	-W, --width <size>        Browser width, in pixels (Default: 2880)
	-H, --height <size>       Browser height, in pixels (Default: 1620)
	-o, --out <file>          Output file (Default: output.png)
	-h, --help                Show this help screen
)";
    std::cout << usageText << "\n";
}

/**
 * Gets an argument from the CLI
 * @param shrt short arg (-a). Empty string if no short arg.
 * @param lng long arg (--arg). Empty string if no long arg.
 * @param deflt default value to use. Empty string if no default value.
 * @return value of argument
 */
const std::string & getArg(const std::string & shrt  = "",
                           const std::string & lng   = "",
                           const std::string & deflt = "") {
    ArgsParser & args = ArgsParser::instance();
    try {
        if (!shrt.empty() && args.cmdOptionExists(shrt)) {
            return args.getCmdOption(shrt);
        } else if (!lng.empty() && args.cmdOptionExists(lng)) {
            return args.getCmdOption(lng);
        }
    } catch (const std::invalid_argument & exc) {
        // argument specified but value not, exit on error.
        std::cout << "ERROR: " << exc.what() << "\n";
        printUsage();
        exit(1);
    }
    return deflt;
}

int main(int argc, char ** argv) {
    ArgsParser & args = ArgsParser::instance(argc, argv);

    if (args.cmdOptionExists("-h") || args.cmdOptionExists("--help")) {
        std::cout << "A trivial browser engine.\n";
        printUsage();
        return 0;
    }

    std::ifstream htmlFile(
        getArg("", "--html", "examples/sherpa-webpage.html"));
    std::ifstream cssFile(getArg("", "--css", "examples/sherpa-webpage.css"));
    std::string   output(getArg("-o", "--out", "output.png"));

    std::stringstream buffer;
    buffer << htmlFile.rdbuf();
    HTMLParser htmlParser(buffer.str());  // read HTML
    buffer.clear();
    buffer.str(std::string());  // clear buffer
    buffer << cssFile.rdbuf();
    CSSParser cssParser(buffer.str());  // read CSS

    Layout::Rectangle browser(0,
                              0,
                              std::stoull(getArg("-W", "--width", "2880")),
                              std::stoull(getArg("-H", "--height", "1620")));

    // compute internal representations
    auto domTree    = htmlParser.evaluate();
    auto stylesheet = cssParser.evaluate();
    auto styleTree  = Style::StyledNode::from(domTree, stylesheet);
    auto layoutTree = Layout::Box::from(styleTree,
                                        Layout::BoxDimensions(browser));

    // initialize magick
    Magick::InitializeMagick(*argv);

    // create canvas
    Canvas canvas(layoutTree, browser);

    // read in pixel data from Canvas
    Magick::Image image;
    image.read(static_cast<uint64_t>(browser.width),
               static_cast<uint64_t>(browser.height),
               "RGBA",
               Magick::CharPixel,
               canvas.getPixels().data());

    // Write the image to output
    image.write(output);

    std::cout << "Output written to " << output << ".\n";
}
