#ifndef FACTORY_HPP
#define FACTORY_HPP

class Factory
{
public:
    Factory();
    virtual ~Factory();
    // This virtual is very important
    // If you don't have this virtual, try this code
    // Factory *f = new ComputerFactory;
    // delete f;
    // ComputerFactory's destructor will not be called, causing a memory leak

    virtual void name();
};

enum FactoryType { Computer, Phone };

auto createFactory(FactoryType type) -> Factory *;

#endif // FACTORY_HPP
