#pragma once

#include <iostream>
#include <memory>
#include <string>

class Controller;

class Model
{
public:
    Model() {}
    ~Model() {}

    void setID(int id) { m_id = id; }
    int ID() const { return m_id; }

    void setName(const std::string &name) { m_name = name; }
    const std::string &name() const { return m_name; }

private:
    int m_id = 0;
    std::string m_name;
};

class View
{
public:
    View() {}
    ~View() {}

    void setcontroller(std::shared_ptr<Controller> controller) { m_controllerPtr = controller; }

    void setIdAndName(int id, const std::string &name);

    void print(int id, const std::string &name)
    {
        std::cout << "ID: " << id << std::endl;
        std::cout << "Name: " << name << std::endl;
    }

private:
    std::weak_ptr<Controller> m_controllerPtr;
};

class Controller
{
public:
    Controller() {}
    ~Controller() {}

    void setModel(std::shared_ptr<Model> model) { m_modelPtr = model; }
    void setView(std::shared_ptr<View> view) { m_viewPtr = view; }

    void update(int id, const std::string &name)
    {
        auto model = m_modelPtr.lock();
        if (model) {
            model->setID(id);
            model->setName(name);
        }
        auto view = m_viewPtr.lock();
        if (view) {
            view->print(model->ID(), model->name());
        }
    }

private:
    std::weak_ptr<Model> m_modelPtr;
    std::weak_ptr<View> m_viewPtr;
};
