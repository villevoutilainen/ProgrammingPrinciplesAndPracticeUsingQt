#include "PPP/Window.h"
#include "PPP/Graph.h"

using namespace Graph_lib;
int main(int /*argc*/, char* /*argv*/[])
{
    Application app;
    Window window({20, 20}, 600, 400, "Main window");
    Text text({150, 20}, "Well hello there!");
    window.attach(text);
    app.gui_main();
}
