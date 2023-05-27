#include "model.hpp"

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    auto model = std::make_shared<Model>();
    auto view = std::make_shared<View>();
    auto controller = std::make_shared<Controller>();

    view->setcontroller(controller);

    controller->setModel(model);
    controller->setView(view);

    controller->update(1, "test");
    controller->update(2, "test2");
    controller->update(3, "test3");

    view->setIdAndName(11, "test1");
    view->setIdAndName(21, "test21");

    return 0;
}