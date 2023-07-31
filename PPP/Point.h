
#ifndef POINT_GUARD
#define POINT_GUARD

//typedef void (*Callback)(void*,void*);

namespace Graph_lib {

struct Point {
	int x,y;
};

inline bool operator==(Point a, Point b) { return a.x==b.x && a.y==b.y; }

inline bool operator!=(Point a, Point b) { return !(a==b); }


}
#endif
