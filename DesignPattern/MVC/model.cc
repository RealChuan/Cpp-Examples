#include "model.hpp"

void View::setIdAndName(int id, const std::string &name)
{
    auto controller = m_controllerPtr.lock();
    if (controller) {
        controller->update(id, name);
    }
}
