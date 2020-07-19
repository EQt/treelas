#include "argparser.hpp"


void
ArgParser::add_option(char short_name, const char *long_name,
                      const char *description,
                      const char *param_value,
                      const char *default_value)
{
    if (short2long_names.count(short_name) > 0)
        throw std::invalid_argument(std::string("short option with '") +
                                    short_name + "' already exists");
    if (options.count(long_name) > 0)
        throw std::invalid_argument(std::string("long option with --") +
                                    long_name + " already exists");
    if (long_name == nullptr)
        throw std::runtime_error("long_name == NULL");
    options[long_name] = {short_name, description, param_value, default_value};
    long_names.push_back(long_name);
    short2long_names[short_name] = long_name;
    desc_width = std::max(desc_width, std::string(long_name).length() +
                          std::string(description).length());
}


ArgParser::ArgParserException::ArgParserException(const char *arg, int index)
    : std::runtime_error("Could not parse argument nr " + std::to_string(index) +
                         ": \"" + std::string(arg ? arg : "(nullptr)") + "\"")
{}


void
ArgParser::parse(int *argc, char *argv[])
{
    if (include_help)
        this->add_option(short2long_names.count('h') > 0 ? '\0' : 'h',
                         "help", "Print this help");
    std::vector<char *> unparsed_args;
    for (int i = 1; i < *argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] != '\0') {
            if (!parse_arg(argv, &i))
                throw ArgParserException(argv[i], i);
        } else {
            unparsed_args.push_back(argv[i]);
        }
    }
    *argc = int(unparsed_args.size()+1);
    for (int i = 1; i < *argc; i++) {
        argv[i] = unparsed_args[i-1];
    }
    argv[*argc] = nullptr;

    if (this->has_option("help")) {
        this->print_usage();
        std::exit(1);
    }
}


bool
ArgParser::parse_long_option(const char *long_name, char *argv[], int *i)
{
    if (options.count(long_name) <= 0)
        return false;

    Option &o = options[long_name];
    if (o.param_value) { // additional parameter?
        *i = *i+1;
        const char *arg_next = argv[*i];
        if (arg_next == nullptr || arg_next[0] == '-') {
            throw ArgParserException(argv[*i], *i);
        }
        o.value = arg_next;
    } else {
        o.value = argv[*i];
    }
    return true;
}


bool
ArgParser::parse_arg(char *argv[], int *i)
{
    const char *arg_i = &argv[*i][1];
    const char short_name = arg_i[0];
    const char *long_name = arg_i+1;
    if (short_name != '-') { // short name
        if (short2long_names.count(short_name) <= 0)
            return false;
        long_name = short2long_names[arg_i[0]];
    }
    return parse_long_option(long_name, argv, i);
}


const char*
ArgParser::get_option(const char *long_name)
{
    if (options.count(long_name) <= 0)
        return nullptr;
    return options[long_name].value;
}


bool
ArgParser::has_option(const char *long_name)
{
    if (options.count(long_name) <= 0)
        return false;
    return options[long_name].value != nullptr;
}


/**
 * Wrap long lines of str (split after ' ' or '\n').
 */
std::string
wrap_line(const std::string &str,
          size_t start_pos = 0,
          size_t indent = 0,
          size_t line_width = 80)
{
    std::string res ("");
    size_t pos = 0, next = 0, line_length = start_pos;
    bool line_empty = true;
    while (pos < str.length() && next != std::string::npos) {
        next = str.find_first_of(" \n\t", pos);
        const auto word_length = next - pos;
        const auto word = str.substr(pos, word_length);
        if (line_length + 1 + word_length < line_width) { // word fits
            if (!line_empty) {
                res += " ";
                line_length++;
            }
        } else {                                          // need line break
            std::string indent_str;
            indent_str.resize(indent, ' ');
            res += "\n" + std::string(indent_str);
            line_length = indent;
        }
        res += word;
        line_length += word_length;
        line_empty = false;
        pos = next+1;
    }
    return res;
}


void
ArgParser::print_usage(FILE *out,
                       const size_t text_width,
                       const size_t desc_indent)
{
    fprintf(out,
            "%s\n"
            "Options\n", usage);
    const int long_param_len = int(std::min(desc_width + 2, desc_indent - 7));
    for (const auto &ln : long_names) {
        Option &o = options[ln];
        std::string long_param = ln;
        if (o.param_value)
            long_param += " " + std::string(o.param_value);
        const auto pos = std::max(4 + 2 + long_param.length(), desc_indent);
        const auto desc = wrap_line(o.description, pos, desc_indent, text_width);
        if (o.short_name)
            fprintf(out, " -%c --%-*s %s\n",
                    o.short_name, long_param_len, long_param.c_str(), desc.c_str());
        else
            fprintf(out, "    --%-*s %s\n",
                    long_param_len, long_param.c_str(), desc.c_str());
    }
}
