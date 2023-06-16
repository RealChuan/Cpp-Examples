#include "observer.hpp"

auto main(int argc, char *argv[]) -> int
{
    (void) argc;
    (void) argv;

    auto phoneObserver = std::make_shared<PhoneObserver>();
    auto computerObserver = std::make_shared<ComputerObserver>();

    auto concreteSubject = std::make_unique<ConcreteSubject>();
    concreteSubject->attach(phoneObserver);
    concreteSubject->attach(computerObserver);

    concreteSubject->setState(1);
    concreteSubject->notify();
    std::cout << "----------------------------" << std::endl;
    concreteSubject->detach(computerObserver);
    concreteSubject->setState(222);
    concreteSubject->notify();
    std::cout << "----------------------------" << std::endl;
    concreteSubject->attach(computerObserver);
    concreteSubject->detach(phoneObserver);
    concreteSubject->setState(333);
    concreteSubject->notify();
    std::cout << "----------------------------" << std::endl;
    concreteSubject->attach(phoneObserver);
    concreteSubject->setState(444);
    concreteSubject->notify();
    std::cout << "----------------------------" << std::endl;
    phoneObserver.reset();
    computerObserver.reset();
    concreteSubject->setState(555);
    concreteSubject->notify();

    return 0;
}
