#pragma once

namespace gplot::graphics
{
    class Renderer
    {
    public:

        void PrepareFrame();

        void RenderCurrent();

        void ResizeWindowCanvas(int width, int height);

    };
}