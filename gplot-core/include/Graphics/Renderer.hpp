#pragma once

#include <Graphics/Shader.hpp>
#include <Graphics/RenderTypes.hpp>
#include <Graphics/VertexBuffer.hpp>

namespace gplot::graphics
{
    class Renderer
    {
    public:

        void PrepareFrame();

        void RenderCurrent();

        void ResizeWindowCanvas(int width, int height);

        void RenderMultiBuffer(Primitive primitive, VertexBuffer& buffer, const std::vector<size_t>& offsets, const std::vector<size_t>& sizes, size_t count);

    };
}