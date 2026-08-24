#include <emscripten/bind.h>

class b2Body {
private:
    ~b2Body() {}
};

namespace emscripten {
    template <>
    inline void raw_destructor<b2Body>(b2Body*) {}
}

EMSCRIPTEN_BINDINGS(test) {
    emscripten::class_<b2Body>("b2Body");
}
