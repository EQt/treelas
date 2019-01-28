#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <cstdio>

/**
 * Simple command line argument parser.
 */
class ArgParser
{
public:
    ArgParser(const char *usage, bool include_help=true)
        : usage(usage), include_help(include_help) { }

    /** Add new option.
     * If param_value != NULL accept this option only with a value */
    void add_option(char short_name, const char *long_name,
                    const char *description,
                    const char *param_value = nullptr,
                    const char *default_value = nullptr);

    class ArgParserException : public std::runtime_error
    {
    public:
        ArgParserException(const char *arg, int index);
    };

    /** Parse command line and update argc and argv accordingly.*/
    void parse(int *argc, char *argv[]);

    /** return value of option with long_name or NULL if not set*/
    const char* get_option(const char *long_name);

    /** return true if option is set */
    bool has_option(const char *long_name);

    /**
     * print first usage and then all options formatted
     * by line with length <= text_width
     * desc_indent = indentation until description */
    void print_usage(FILE *out = stdout,
                     const size_t text_width  = 80,
                     const size_t desc_indent = 25);

private:
    const char *usage;
    bool include_help;
    size_t desc_width = 0;  /** max width of long_name + descriptionn */

    struct Option
    {
        char short_name;
        const char *description;
        const char *param_value;
        const char *value;
    };
    std::unordered_map<char, const char*> short2long_names;
    std::unordered_map<std::string, Option> options;
    std::vector<const char *> long_names;

    bool parse_arg(char *argv[], int *i);
    bool parse_long_option(const char *long_name, char *argv[], int *i);
};

#endif /* ARGPARSER_HPP */
