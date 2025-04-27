#pragma once

#include <iostream>
#include <list>
#include <memory>
#include <mutex>

class Observer : public std::enable_shared_from_this<Observer>
{
public:
    virtual ~Observer() {}

    virtual void update(int) = 0;
};

class Subject
{
public:
    virtual ~Subject() {}

    virtual void attach(std::weak_ptr<Observer>) = 0;
    virtual void detach(std::weak_ptr<Observer>) = 0;
    virtual void notify() = 0;
};

class PhoneObserver : public Observer
{
public:
    virtual void update(int i) { std::cout << "This is PhoneObserver " << i << std::endl; }
};

class ComputerObserver : public Observer
{
public:
    virtual void update(int i) { std::cout << "This is ComputerObserver " << i << std::endl; }
};

class ConcreteSubject : public Subject
{
public:
    virtual void attach(std::weak_ptr<Observer> observerPtr) { m_Observers.push_back(observerPtr); }
    virtual void detach(std::weak_ptr<Observer> observerPtr)
    {
        m_Observers.remove_if(
            [=](const std::weak_ptr<Observer> &ptr1) { return observerPtr.lock() == ptr1.lock(); });
    }
    virtual void notify()
    {
        for (auto iter = m_Observers.begin(); iter != m_Observers.end();) {
            auto share = (*iter).lock();
            if (share) {
                share->update(m_state);
                iter++;
            } else {
                iter = m_Observers.erase(iter);
            }
        }
    }

    void setState(int state) { m_state = state; }

private:
    std::list<std::weak_ptr<Observer>> m_Observers;
    int m_state = 0;
};
