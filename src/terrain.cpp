#include "terrain.h"
#include <math.h>
#include <random>
#include <string>
#include "mat.h"
#include "image.h"
#include "image_io.h"


Terrain2D::Terrain2D(InfinitTexture2D *texture, vec2 min_p, vec2 max_p, int nx, int ny){
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

Terrain2D::Terrain2D(const ScalarField2D& sf){
    values = sf.get_values();
    nx = sf.get_nx();
    ny = sf.get_ny();
    min_p = sf.get_min_p();
    max_p = sf.get_max_p();
    slope_max = max_slope();
}

Terrain2D::Terrain2D(const char *filename, vec2 min_p, vec2 max_p){
    Image image = read_image(filename);
    this->nx = image.width();
    this->ny = image.height();
    this->min_p = Point(min_p.x, min_p.y, 0);
    this->max_p = Point(max_p.x, max_p.y, 0);
    values = std::vector<float>(nx*ny);

    for(int j = 0; j < ny; j++){
        for(int i = 0; i < nx; i++){
            float x = float(i)/(nx-1) * (max_p.x-min_p.x) + min_p.x;
            float y = float(j)/(ny-1) * (max_p.y-min_p.y) + min_p.y;
            float z = image(i, j).r;
            set_value(i, j, z);
            if(this->min_p.z > z)
                this->min_p.z = z;
            if(this->max_p.z < z)
                this->max_p.z = z;
        }
    }
    slope_max = max_slope();
}





neighborhood Terrain2D::find_neighborhood(int i, int j, float p) const{
    neighborhood neighbors = neighborhood();
    neighbors.size = 0;
    
    float total_neighbor_coef = 0;

    for(int i_ = -1; i_ < 2; i_++){
        for(int j_ = -1; j_ < 2; j_++){
            if(i+i_ >= 0 && i+i_ < nx && j+j_ > 0 && j+j_ < ny && (i_!=0 || j_!=0)){
                neighbors.x[neighbors.size] = i_;
                neighbors.y[neighbors.size] = j_;
                neighbors.length[neighbors.size] = sqrt(i_*i_ + j_*j_);

                // neighbors.coef[neighbors.size] = get_value(i, j) - get_value(i+i_, j+j_);
                neighbors.coef[neighbors.size] = std::max(0.0f, get_value(i, j) - get_value(i+i_, j+j_));
                neighbors.coef[neighbors.size] = pow(neighbors.coef[neighbors.size], p);
                total_neighbor_coef+=neighbors.coef[neighbors.size];

                neighbors.size++;
            }
        }
    }
    //normalisatoion
    if(total_neighbor_coef != 0)
        for(int i = 0; i < neighbors.size; i++)
            neighbors.coef[i]/=total_neighbor_coef;
        
    return neighbors;
}

float Terrain2D::height(int i, int j) const{
    return get_value(i, j);
}
float Terrain2D::height(float x, float y) const{
    float i = nx*(x-min_p.x)/(max_p.x-min_p.x);
    float j = ny*(y-min_p.y)/(max_p.y-min_p.y);
    if(i < 0 || i > nx || j < 0 || j > ny)
        return 0;
    
    int i_0 = floor(i);
    float s_i = i - i_0;
    int j_0 = floor(j);
    float s_j = j - j_0;

    //top and right border case
    if(s_i == 0 && s_j == 0)
        return get_value(i_0, j_0);
    if(s_i == 0)
        return get_value(i_0, j_0)*(1-s_j)
             + get_value(i_0, j_0+1)*s_j;
    if(s_j == 0)
        return get_value(i_0, j_0)*(1-s_i)
             + get_value(i_0+1, j_0)*s_i;

    return get_value(i_0, j_0)*(1-s_i)*(1-s_j)
         + get_value(i_0, j_0+1)*(1-s_i)*s_j
         + get_value(i_0+1, j_0)*s_i*(1-s_j)
         + get_value(i_0+1, j_0+1)*s_i*s_j;
}

Point Terrain2D::point(int i, int j) const{
    float x = float(i)/(nx-1) * (max_p.x-min_p.x) + min_p.x;
    float y = float(j)/(ny-1) * (max_p.y-min_p.y) + min_p.y;
    return Point(x, y, height(i, j));
}

vec2 Terrain2D::gradient(int i, int j) const{
    //gradient in x
    float gradient_x;
    float gradient_y;
    float ex = (max_p.x - min_p.x)/(nx-1);
    float ey = (max_p.y - min_p.y)/(ny-1);

    if(i == 0){
        gradient_x = (height(i+1, j) - height(i, j))/ex;
    }else if(i == nx-1){
        gradient_x = (height(i, j) - height(i-1, j))/ex;
    }else{
        gradient_x = (height(i+1, j) - height(i-1, j))/(2*ex);
    }

    if(j == 0){
        gradient_y = (height(i, j+1) - height(i, j))/ey;
    }else if(j == ny-1){
        gradient_y = (height(i, j) - height(i, j-1))/ey;
    }else{
        gradient_y = (height(i, j+1) - height(i, j-1))/(2*ey);
    }
    return vec2(gradient_x, gradient_y);
}

Vector Terrain2D::normal(int i, int j) const{
    vec2 grad = gradient(i, j);
    return normalize(Vector(-grad.x, -grad.y, 1));
}

float Terrain2D::slope(int i, int j) const{
    vec2 grad = gradient(i, j);
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
    int nb_ray = 64;
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

std::vector<vec2> Terrain2D::get_texcoords() const{
    std::vector<vec2> texcoords = std::vector<vec2>();
    for(int j = 0; j < ny; j++)
        for(int i = 0; i < nx; i++)
            texcoords.push_back(vec2(float(i)/(nx-1), float(j)/(ny-1)));
    return texcoords;
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

ScalarField2D Terrain2D::get_slopes() const{
    std::vector<float> slopes = std::vector<float>(values.size());
    for(int j = 0; j < ny; j++)
        for(int i = 0; i < nx; i++)
            slopes[get_index(i, j)] = slope(i, j);
    return ScalarField2D(slopes, nx, ny, min_p, max_p);
}

ScalarField2D Terrain2D::get_occlusions() const{
    std::vector<float> ambiants = std::vector<float>(values.size());
    
    // #pragma omp parallel for
    for(int j = 0; j < ny; j++)
        for(int i = 0; i < nx; i++)
            ambiants[get_index(i, j)] = ambiant_occlusion(i, j);
    return ScalarField2D(ambiants, nx, ny, min_p, max_p);
}

ScalarField2D Terrain2D::get_drains(float p) const{

    std::vector<float> drain = std::vector<float>(values.size(), 1.0f);
    std::vector<heighCell> cells = std::vector<heighCell>(nx*ny);
    for(int i = 0; i < nx; i++){
        for(int j = 0; j < ny; j++){
            cells[get_index(i, j)].height = get_value(i, j);
            cells[get_index(i, j)].i = i;
            cells[get_index(i, j)].j = j;
        }
    }
    std::sort(cells.begin(), cells.end(), [](heighCell a, heighCell b) {
        return a.height > b.height;
    });
    for(heighCell cell : cells){
        int i = cell.i;
        int j = cell.j;
        neighborhood neighbors = find_neighborhood(i, j, p);
        for(int k = 0; k < neighbors.size; k++)
            drain[get_index(neighbors.x[k]+i, neighbors.y[k]+j)] += neighbors.coef[k]*drain[get_index(i, j)];
    }

    return ScalarField2D(drain, nx, ny, min_p, max_p);
}