
#include "simple_app.h"
#include "noise.h"
#include "scalar_field.h"
#include "infinit_texture.h"
#include "terrain.h"

std::vector<std::vector<float>> gaussian_kernel(int size, float sigma = 1.0f){
    float coef = 1.0/(2*M_PI*sigma*sigma);
    std::vector<std::vector<float>> kernel = std::vector<std::vector<float>>();
    for(int i =-size/2; i <= size/2; i++){
        kernel.push_back(std::vector<float>());
        for(int j = -size/2; j <= size/2; j++){
            kernel[i+size/2].push_back(coef*exp(-(i*i + j*j)/(2*sigma*sigma)));
        }
    }
    return kernel;
}

void erosion(Terrain2D& t){
    for(int _ = 0; _ < 100; _++)
        t+=0.0001 + (-0.00005)*t.get_drains().map([](float x){return float(pow(x, 0.5));})*t.get_slopes().map([](float x){return float(pow(x, 0.8));}) + (0.0001)*t.laplacian();
}

void hill(Terrain2D& t){
    for(int _ = 0; _ < 1500; _++)
        t+= (0.0001)*t.laplacian();
}

int main( int argc, char **argv )
{

    InfinitTexture2D* noise = new InfinitTexture2DFromNoise(new ValueGrid2D());
    noise = translation(noise, vec2(23, -34));
    noise = zoom(noise, vec2(0.5, 0.5));
    noise = scale(noise, 2);
    noise = rotation(noise, M_PI/7);
    noise = sum(noise, new InfinitTexture2DFromNoise(new ValueGrid2D()));

    int res = 256;
    Terrain2D t = Terrain2D(noise, vec2(-5, -5), vec2(5, 5), res, res);
    std::vector<std::vector<float>> kernel = gaussian_kernel(11, 3);

    Terrain2D t_ = Terrain2D(t.convolution(kernel));

    t_.export_as_image("mean.png");
    

    
    //Geologic equation:
    erosion(t_);

    //THIS IS WORKING, it smooth out really well the terrain
    // hill(t_);


    ScalarField2D l = t_.derivate_x() + t_.derivate_y();
    l.export_as_image("derivate.png");

    // t.get_occlusion_color();
    // t.export_as_image("test.png");
    // noise->export_as_image("noise.png");
    t_.get_slopes().export_as_image("slope.png");
    t_.get_drains().map([](float x){return sqrt(x);}).export_as_image("drain.png");
    (t_.get_drains(0.1)*t_.get_slopes()).map([](float x){return sqrt(x);}).export_as_image("drain and slope.png");
    t_.laplacian().export_as_image("laplacian.png");
    t_.export_as_image("height.png");
    if(false)t_.get_occlusions().export_as_image("occlusion.png", false);

    const char* texture_file = "diff.png";
    // SimpleApp simple_app = SimpleApp(1024, 640, t_.get_positions(), t_.get_normals(), t_.get_texcoords(), texture_file, t_.get_indexes());
    SimpleApp simple_app = SimpleApp(1024, 640, t_.get_positions(), t_.get_normals(), t_.get_indexes());
    
    // SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(),t.get_slopes().get_values_as_color(), t.get_indexes());
    
    // SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(),t.get_slope_color(), t.get_indexes());
    // SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(),t.get_occlusion_color(), t.get_indexes());
    simple_app.run();
    
    return 0;
}
