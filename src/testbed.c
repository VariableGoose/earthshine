#define ES_VULKAN
#include "es_header.h"

i32_t main(void) {
    es_formatter_init();

    es_log("{f32 -1.3} {i32}\n", 3.1415926f);
    es_log("{i32 +13}\n", -3213);
    es_log("{u32 -13}\n", 3123);
    es_log("{str 32}\n", "leif");

    es_log("Math:\n");
    es_log("    Vec2: {vec2 1.0}\n", vec2(1, 2));
    es_log("    Vec3: {vec3 1.0}\n", vec3(1, 2, 3));
    es_log("    Vec4: {vec4 1.0}\n", vec4(1, 2, 3, 4));

    es_formatter_free();
    return 0;
}
