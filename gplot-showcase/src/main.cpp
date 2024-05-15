#include <include/AppLayer.hpp>
#include <Core/Application.hpp>

int main(int argc, char* argv[])
{
    gplot::core::Application app;
    app.AddLayer<gplot::AppLayer>();
    app.Run();
    return 0;
}
