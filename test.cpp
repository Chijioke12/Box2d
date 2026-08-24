#include <emscripten/bind.h>
#include <emscripten/val.h>

class b2Shape {};
class b2FixtureDef { public: b2Shape* shape; };

emscripten::val get_shape(const b2FixtureDef& def) {
    return emscripten::val(def.shape, emscripten::allow_raw_pointers());
}
void set_shape(b2FixtureDef& def, emscripten::val shape) {
    def.shape = shape.as<b2Shape*>(emscripten::allow_raw_pointers());
}

EMSCRIPTEN_BINDINGS(test) {
    emscripten::class_<b2Shape>("b2Shape");
    emscripten::class_<b2FixtureDef>("b2FixtureDef")
        .property("shape", &get_shape, &set_shape);
}
