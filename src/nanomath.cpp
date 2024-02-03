#include "nanomath.hpp"
#include <argparse/argparse.hpp>

int main(int argc, char *argv[])
{
    argparse::ArgumentParser parser{"nanomath"};
    parser.add_argument("--format").help("output format [ns | ms | s | m | h | D | W | M | Y]");
    parser.add_argument("expression");

    std::string expression, format;
    bool formatted = false;
    try
    {
        parser.parse_args(argc, argv);
        expression = parser.get<std::string>("expression");
        formatted = parser.is_used("--format");
        if (formatted)
            format = parser.get<std::string>("--format");
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        const auto ns = nanomath::parse(expression);
        if (formatted)
            std::cout << nanomath::format(ns, nanomath::string_to_unit(format)) << std::endl;
        else
            std::cout << nanomath::format(ns) << std::endl;
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
