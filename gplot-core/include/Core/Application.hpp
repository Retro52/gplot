#pragma once

#include <Core/Layer.hpp>

#include <vector>
#include <memory>

namespace gplot::core
{
    class Application
    {
    public:

        ~Application() noexcept;

        void Run();

        template<typename T, typename... Args>
        void AddLayer(Args&&... args)
        {
            static_assert(std::is_base_of_v<Layer, T>);

            auto layer = std::make_shared<T>(std::forward<Args>(args)...);
            layer->OnAttach();
            m_layers.push_back(layer);
        }

    private:

        std::vector<std::shared_ptr<Layer>> m_layers;

    };
}
