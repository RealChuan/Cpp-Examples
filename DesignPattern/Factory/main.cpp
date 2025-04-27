#include "factory.hpp"

#include <memory>

auto main(int argc, char *argv[]) -> int
{
    (void) argc;
    (void) argv;

    std::unique_ptr<Factory> computerFactoryPtr(createFactory(FactoryType::Computer));
    std::unique_ptr<Factory> phoneFactoryPtr(createFactory(FactoryType::Phone));
    // std::unique_ptr<Factory> invalidFactoryPtr(createFactory(FactoryType(-1)));

    return 0;
}
