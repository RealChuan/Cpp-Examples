#include "factory.hpp"

#include <memory>

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    std::unique_ptr<Factory> f(createFactory(FactoryType::Computer));
    std::unique_ptr<Factory> f1(createFactory(FactoryType::Phone));
    //std::unique_ptr<Factory> f2(createFactory(FactoryType(-1)));

    return 0;
}
