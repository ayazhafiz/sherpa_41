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
 * Prints help menu
 */
void printHelp() {
    static std::string helpText = R"(
A trivial browser engine.

USAGE: sherpa_41 [options]

OPTIONS:
	--html                  HTML file to parse (=assets/rainbowBox.html)
	--css                   CSS file to parse (=assets/rainboxBox.css)
	-w, --width             Browser width (=800)
	-h, --height            Browser height (=600)
	-o, --out               Output file (=output.png)
	--help                  Show this help screen
)";
    std::cout << helpText << std::endl;
}

/**
 * Gets input files from CLI args
 * @param html html file
 * @param css css file
 */
void getInFiles(std::ifstream & html, std::ifstream & css) {
    ArgsParser & args         = ArgsParser::instance();
    auto         htmlFileName = args.getCmdOption("--html");
    auto         cssFileName  = args.getCmdOption("--css");
    if (!htmlFileName.empty()) {
        html = std::ifstream(htmlFileName);
    }
    if (!cssFileName.empty()) {
        css = std::ifstream(cssFileName);
    }
}

/**
 * Gets output files from CLI args
 * @param out output file
 */
void getOutFiles(std::string & out) {
    ArgsParser & args           = ArgsParser::instance();
    auto         outputFileName = args.cmdOptionExists("-o")
                              ? args.getCmdOption("-o")
                              : args.getCmdOption("--out");
    if (!outputFileName.empty()) {
        out = outputFileName;
    }
}

/**
 * Converts a dimension in a CLI arg
 * @param shrt short arg (-a)
 * @param lng long arg (--arg)
 * @param deflt default value
 * @return converted dimension
 */
uint64_t getDimension(const std::string & shrt,
                      const std::string & lng,
                      uint64_t            deflt) {
    ArgsParser & args = ArgsParser::instance();
    if (args.cmdOptionExists(shrt)) {
        return std::stoull(args.getCmdOption(shrt));
    } else if (args.cmdOptionExists(lng)) {
        return std::stoull(args.getCmdOption(lng));
    }
    return deflt;
}

int main(int argc, char ** argv) {
    ArgsParser & args = ArgsParser::instance(argc, argv);

    if (args.cmdOptionExists("--help")) {
        printHelp();
        return 0;
    }

    std::ifstream htmlFile("examples/test.html");
    std::ifstream cssFile("examples/test.css");
    getInFiles(htmlFile, cssFile);

    std::string output("output.png");
    getOutFiles(output);

    std::stringstream buffer;
    buffer << htmlFile.rdbuf();
    HTMLParser htmlParser(buffer.str());  // read HTML
    buffer.clear();
    buffer.str(std::string());  // clear buffer
    buffer << cssFile.rdbuf();
    CSSParser cssParser(buffer.str());  // read CSS

    Layout::Rectangle browser(0,
                              0,
                              getDimension("-w", "--width", 2880),
                              getDimension("-h", "--height", 1800));

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

    std::cout << "Output written to " << output << "." << std::endl;
}