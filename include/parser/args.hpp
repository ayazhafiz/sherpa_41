// sherpa_41's Args Parser, licensed under MIT. (c) hafiz, 2018

#ifndef PARSER_ARGS_HPP
#define PARSER_ARGS_HPP

#include <string>
#include <vector>

/**
 * A trivial CLI Args parser
 * @cite https://stackoverflow.com/a/868894
 * @author iain
 */
class ArgsParser {
   public:
    ArgsParser(const ArgsParser &) = delete;

    static ArgsParser & instance(int argc = 0, char ** argv = 0) {
        if (parser == nullptr) {
            parser = new ArgsParser(argc, argv);
        }
        return *parser;
    }
    /// @author iain
    const std::string & getCmdOption(const std::string & option) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }
    /// @author iain
    bool cmdOptionExists(const std::string & option) const {
        return std::find(this->tokens.begin(), this->tokens.end(), option) !=
               this->tokens.end();
    }

   private:
    ArgsParser(int argc, char ** argv) {
        for (int i = 1; i < argc; ++i)
            this->tokens.emplace_back(std::string(argv[i]));
    }

    static ArgsParser * parser;

    std::vector<std::string> tokens;
};

ArgsParser * ArgsParser::parser = nullptr;

#endif