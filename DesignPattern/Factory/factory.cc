#include "factory.hpp"

#include <iostream>

Factory::Factory() {}

Factory::~Factory()
{
    std::cout << "~Factory" << std::endl;
}

void Factory::name()
{
    std::cout << "Factory" << std::endl;
}

class ComputerFactory : public Factory
{
public:
    ComputerFactory()
        : Factory()
    {}
    virtual ~ComputerFactory() { std::cout << "~ComputerFactory" << std::endl; }

    virtual void name() { std::cout << "ComputerFactory" << std::endl; }
};

class PhoneFactory : public Factory
{
public:
    PhoneFactory()
        : Factory()
    {}
    virtual ~PhoneFactory() { std::cout << "~PhoneFactory" << std::endl; }

    virtual void name() { std::cout << "PhoneFactory" << std::endl; }
};

auto createFactory(FactoryType type) -> Factory *
{
    switch (type) {
    case Computer: return new ComputerFactory;
    case Phone: return new PhoneFactory;
    default: break;
    }
    return new Factory;
}
