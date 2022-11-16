#include "scalar_field.h"
#include <math.h>
#include <random>
#include "mat.h"

ScalarField2D::ScalarField2D():values(){}
ScalarField2D::ScalarField2D(const std::vector<float>& values, int nx, int ny):values(values), nx(nx), ny(ny){}

float ScalarField2D::get_value(int i, int j) const{
    return values[j*nx + i];
}

void ScalarField2D::set_value(int i, int j, float v){
    values[j*nx + i] = v;
}

Terrain2D::Terrain2D(InfinitTexture2D *texture, vec2 min_p, vec2 max_p, int nx, int ny): texture(texture){
    this->nx = nx;
    this->ny = ny;
    this->min_p = Point(min_p.x, min_p.y, 0);
    this->max_p = Point(max_p.x, max_p.y, 0);
    values = std::vector<float>(nx*ny);

    for(int j = 0; j < ny; j++){
        for(int i = 0; i < nx; i++){
            float x = float(i)/(nx-1) * (max_p.x-min_p.x) + min_p.x;
            float y = float(j)/(ny-1) * (max_p.y-min_p.y) + min_p.y;
            float z = texture->value(x, y);
            set_value(i, j, z);
            if(this->min_p.z > z)
                this->min_p.z = z;
            if(this->max_p.z < z)
                this->max_p.z = z;
        }
    }
    slope_max = max_slope();
}

float Terrain2D::height(int i, int j) const{
    return get_value(i, j);
}
float Terrain2D::height(float x, float y) const{
    return texture->value(x, y);
}

Point Terrain2D::point(int i, int j) const{
    float x = float(i)/(nx-1) * (max_p.x-min_p.x) + min_p.x;
    float y = float(j)/(ny-1) * (max_p.y-min_p.y) + min_p.y;
    return Point(x, y, height(i, j));
}

vec2 Terrain2D::gradiant(int i, int j) const{
    //gradient in x
    float gradiant_x;
    float gradiant_y;
    float ex = (max_p.x - min_p.x)/(nx-1);
    float ey = (max_p.y - min_p.y)/(ny-1);

    if(i == 0){
        gradiant_x = (height(i+1, j) - height(i, j))/ex;
    }else if(i == nx){
        gradiant_x = (height(i, j) - height(i-1, j))/ex;
    }else{
        gradiant_x = (height(i+1, j) - height(i-1, j))/(2*ex);
    }

    if(j == 0){
        gradiant_y = (height(i, j+1) - height(i, j))/ey;
    }else if(j == ny){
        gradiant_y = (height(i, j) - height(i, j-1))/ey;
    }else{
        gradiant_y = (height(i, j+1) - height(i, j-1))/(2*ey);
    }
    return vec2(gradiant_x, gradiant_y);
}

Vector Terrain2D::normal(int i, int j) const{
    vec2 grad = gradiant(i, j);
    return normalize(Vector(-grad.x, -grad.y, 1));
}

float Terrain2D::slope(int i, int j) const{
    vec2 grad = gradiant(i, j);
    return sqrt(grad.x*grad.x + grad.y*grad.y);
}

float Terrain2D::max_slope() const{
    float slope_max = 0;
    for(int i = 0; i < nx; i++)
        for(int j = 0; j < ny; j++)
            if(slope_max < slope(i, j))
                slope_max = slope(i, j);
    return slope_max;
}

bool Terrain2D::ray_intersection(Point o, Vector d, Point* intersection) const{

    //compute intersection with bouding box
    float tx_min = (min_p.x-o.x)/d.x;
    float tx_max = (max_p.x-o.x)/d.x;
    float ty_min = (min_p.y-o.y)/d.y;
    float ty_max = (max_p.y-o.y)/d.y;
    float tz_min = (min_p.z-o.z)/d.z;
    float tz_max = (max_p.z-o.z)/d.z;

    float t_min = std::min(tx_min, std::min(ty_min, tz_min));
    float t_max = std::max(tx_max, std::max(ty_max, tz_max));

    // no intersection with the bounding box or intersection behind the origine
    if(t_min > t_max || t_max < 0)
        return false;
    
    float t = t_min;
    //o is in the bouding box
    if(t_min < 0)
        t_min = 0;

    //search for an intersection on the segment [o+t_min*d, o+t_max*d]
    int nb_iteration = 0;
    int max_iteration = 1000;
    while(t_min < t_max && nb_iteration < max_iteration){
        nb_iteration++;
        Point p = o + d*t_min;
        float h = abs(p.z-height(p.x, p.y));
        if(h < epsilon){
            *intersection = p;
            return true;
        }
        t_min+= h/slope_max;
    }

    return false;
}

bool Terrain2D::ray_intersection(Point o, Vector d) const{
    return ray_intersection(o, d, new Point());
}

float Terrain2D::ambiant_occlusion(int i, int j) const{
    float ambiant = 0;
    int nb_ray = 8;
    std::uniform_real_distribution<float> u01(0.0, 1.0);
    std::default_random_engine rng(42);
    for(int _ = 0; _ < nb_ray; _++){
        float r1 = u01(rng);
        float r2 = u01(rng);
        Vector n = normal(i, j);
        Vector dir(cos(2*M_PI*r1)*sqrt(1-r2*r2), sin(2*M_PI*r1)*sqrt(1-r2*r2), r2);
        dir = Rotation(Vector(0, 0, 1), n)(dir);
        //float proba_dir = 1/(2*M_PI);

        Point o = point(i, j) + epsilon*n;

        if(!ray_intersection(o, dir))
            ambiant+=1.0/nb_ray;
    }
    return ambiant;
}


std::vector<vec3> Terrain2D::get_positions() const{
    std::vector<vec3> positions = std::vector<vec3>();
    for(int j = 0; j < ny; j++)
        for(int i = 0; i < nx; i++)
            positions.push_back(point(i, j));
    return positions;
}
std::vector<vec3> Terrain2D::get_normals() const{
    std::vector<vec3> normals = std::vector<vec3>();
    for(int j = 0; j < ny; j++)
        for(int i = 0; i < nx; i++)
            normals.push_back(normal(i, j));
    return normals;
}
std::vector<int> Terrain2D::get_indexes() const{
    std::vector<int> indexes = std::vector<int>();
    //value are indexed:
    // ...
    // nx - nx+1 - ...
    // 0  - 1    - ...
    for(int i = 0; i < nx-1; i++){
        for(int j = 0; j < ny-1; j++){
            indexes.push_back((j+0)*nx + (i+0));
            indexes.push_back((j+1)*nx + (i+1));
            indexes.push_back((j+1)*nx + (i+0));

            indexes.push_back((j+0)*nx + (i+0));
            indexes.push_back((j+0)*nx + (i+1));
            indexes.push_back((j+1)*nx + (i+1));            
        }
    }
    return indexes;   
}

std::vector<Color> Terrain2D::get_slope_color() const{
    std::vector<Color> colors = std::vector<Color>();
    for(int j = 0; j < ny; j++)
        for(int i = 0; i < nx; i++)
            colors.push_back(Color(20*slope(i, j)/slope_max));
    return colors;
}

std::vector<Color> Terrain2D::get_occlusion_color() const{
    std::vector<Color> colors = std::vector<Color>();
    
    // #pragma omp parallel for
    for(int j = 0; j < ny; j++)
        for(int i = 0; i < nx; i++){
            //std::cout << "ambiant of (" << i << ", " << j << ")" << std::endl;
            colors.push_back(Color(ambiant_occlusion(i, j)));
        }
    return colors;
}
