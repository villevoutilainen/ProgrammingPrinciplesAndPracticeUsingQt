
#include <PPP/GUI.h>	// for Simple_window only (doesn't really belong in Window.h)

using namespace Graph_lib;

// Simple_window is basic scaffolding for ultra-simple interaction with graphics
// it provides one window with one "next" button for ultra-simple animation

struct Simple_window : Window {
    Simple_window(Point xy, int w, int h, const string& title );
	
    void wait_for_button();

	Button next_button;
private:
};
