#include "nanomath.hpp"
#include <argparse/argparse.hpp>

int main(int argc, char *argv[])
{
    argparse::ArgumentParser parser{"nanomath"};
    parser.add_argument("--format").help("output format [ns | ms | s | m | h | D | W | M | Y]");
    parser.add_argument("expression");

    std::string expression, format;
    const auto formatted = parser.is_used("--format");
    try
    {
        parser.parse_args(argc, argv);
        expression = parser.get<std::string>("expression");
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
    catch (const std::format_error &err)
    {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
