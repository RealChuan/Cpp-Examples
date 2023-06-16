#include "factory.hpp"

#include <memory>

auto main(int argc, char *argv[]) -> int
{
    (void) argc;
    (void) argv;

    std::unique_ptr<Factory> f(createFactory(FactoryType::Computer));
    std::unique_ptr<Factory> f1(createFactory(FactoryType::Phone));
    //std::unique_ptr<Factory> f2(createFactory(FactoryType(-1)));

    return 0;
}
