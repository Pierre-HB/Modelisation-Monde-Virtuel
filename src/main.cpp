
#include "simple_app.h"
#include "noise.h"
#include "scalar_field.h"
#include "infinit_texture.h"
#include "terrain.h"

int main( int argc, char **argv )
{

    InfinitTexture2D* noise = new InfinitTexture2DFromNoise(new ValueGrid2D());
    noise = translation(noise, vec2(23, -34));
    noise = zoom(noise, vec2(0.5, 0.5));
    noise = scale(noise, 2);
    noise = rotation(noise, M_PI/7);
    noise = sum(noise, new InfinitTexture2DFromNoise(new ValueGrid2D()));

    Terrain2D t = Terrain2D(noise, vec2(-5, -5), vec2(5, 5), 250, 250);
    // t.get_occlusion_color();
    std::cout << t.ambiant_occlusion(2, 2) << std::endl;;
    // t.export_as_image("test.png");
    // noise->export_as_image("noise.png");
    t.get_slopes().export_as_image("slope.png");
    t.laplacian().export_as_image("laplacian.png");
    t.export_as_image("height.png");
    // t.get_occlusions().export_as_image("occlusion.png", false);
    const char* texture_file = "occlusion.png";
    SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(), t.get_texcoords(), texture_file, t.get_indexes());
    
    // SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(),t.get_slopes().get_values_as_color(), t.get_indexes());
    
    // SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(),t.get_slope_color(), t.get_indexes());
    // SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(),t.get_occlusion_color(), t.get_indexes());
    simple_app.run();
    
    return 0;
}
