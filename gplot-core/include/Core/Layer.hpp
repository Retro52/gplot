#pragma once

namespace gplot::core
{
    class Layer
    {
    public:

        virtual void OnAttach() = 0;

        virtual void OnDetach() = 0;

        virtual bool OnUpdate() = 0;

    };
}

