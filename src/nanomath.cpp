#include "nanomath.hpp"
#include <argparse/argparse.hpp>
#include <chrono>
#include <cstdlib>
#include <exception>

int main(int argc, char *argv[])
{
    argparse::ArgumentParser parser{"nanomath"};
    parser.add_argument("--format").default_value(std::string{"iso"}).help("output format");
    parser.add_argument("--leap").default_value(false).implicit_value(true);
    parser.add_argument("expression");

    std::string expression, format;
    try
    {
        parser.parse_args(argc, argv);
        expression = parser.get<std::string>("expression");
        format = parser.get<std::string>("--format");
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        const auto duration = nanomath::parse(expression);
        const auto sys_time = std::chrono::sys_time<std::chrono::nanoseconds>(duration);
        std::cout << nanomath::format(sys_time, format) << std::endl;
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
