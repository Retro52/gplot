#include <Core/Application.hpp>

using namespace gplot::core;

Application::~Application() noexcept
{
    for (const auto& layer : m_layers)
    {
        layer->OnDetach();
    }

    m_layers.clear();
}

void Application::Run()
{
    bool terminate = false;
    while (!terminate)
    {
        for (const auto& layer : m_layers)
        {
            terminate |= layer->OnUpdate();
        }
    }
}
