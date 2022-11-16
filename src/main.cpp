
#include "simple_app.h"
#include "noise.h"
#include "scalar_field.h"
#include "infinit_texture.h"
#include "terrain.h"

int main( int argc, char **argv )
{
    std::vector<vec3> positions = std::vector<vec3>({
        vec3(-1, 1, 0),
        vec3(1, 1, 0),
        vec3(-1, -1, 0),
        vec3(1, -1, 0)
    });
    std::vector<vec3> normals = std::vector<vec3>({
        normalize(vec3(0, 0, -1)),
        normalize(vec3(0, 0, -1)),
        normalize(vec3(0, 0, -1)),
        normalize(vec3(0, 0, -1))        
    });
    std::vector<int> indexes = std::vector<int>({
        0, 1, 2,
        1, 3, 2
    });

    // Noise1D n = ValueGrid1D();
    Noise1D *n = new ValueGrid1D();
    ValueGrid1D n_ = ValueGrid1D();
    std::cout << n->value(42) << std::endl;
    std::cout << n_.value(42) << std::endl;

    InfinitTexture2D* noise = new InfinitTexture2DFromNoise(new ValueGrid2D());
    noise = translation(noise, vec2(23, -34));
    noise = zoom(noise, vec2(0.5, 0.5));
    noise = scale(noise, 2);
    noise = rotation(noise, M_PI/7);
    noise = sum(noise, new InfinitTexture2DFromNoise(new ValueGrid2D()));

    Terrain2D t = Terrain2D(noise, vec2(-5, -5), vec2(5, 5), 250, 250);
    // t.get_occlusion_color();
    std::cout << t.ambiant_occlusion(2, 2) << std::endl;;
    t.export_as_image("test.png");
    noise->export_as_image("noise.png");
    SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(), t.get_indexes());
    // SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(),t.get_slope_color(), t.get_indexes());
    // SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(),t.get_occlusion_color(), t.get_indexes());
    simple_app.run();
    
    return 0;
}
