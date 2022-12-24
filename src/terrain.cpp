#include "terrain.h"
#include <math.h>
#include <random>
#include <string>
#include <iterator>
#include "mat.h"
#include "image.h"
#include "image_io.h"



Terrain2D::Terrain2D(InfinitTexture2D *texture, vec2 min_p, vec2 max_p, int nx, int ny, float tree_radius){
    this->nx = nx;
    this->ny = ny;
    this->min_p = Point(min_p.x, min_p.y, 0);
    this->max_p = Point(max_p.x, max_p.y, 0);
    values = std::vector<float>(nx*ny);

    for(int j = 0; j < ny; j++){
        for(int i = 0; i < nx; i++){
            vec2 v = get_vec(i, j);
            float z = texture->value(v.x, v.y);
            set_value(i, j, z);
            if(this->min_p.z > z)
                this->min_p.z = z;
            if(this->max_p.z < z)
                this->max_p.z = z;
        }
    }
    slope_max = max_slope();
}

Terrain2D::Terrain2D(const ScalarField2D& sf, float tree_radius){
    values = sf.get_values();    
    nx = sf.get_nx();
    ny = sf.get_ny();
    min_p = sf.get_min_p();
    max_p = sf.get_max_p();
    slope_max = max_slope();
}

Terrain2D::Terrain2D(const char *filename, vec2 min_p, vec2 max_p, float tree_radius){
    Image image = read_image(filename);
    this->nx = image.width();
    this->ny = image.height();
    this->min_p = Point(min_p.x, min_p.y, 0);
    this->max_p = Point(max_p.x, max_p.y, 0);
    values = std::vector<float>(nx*ny);

    for(int j = 0; j < ny; j++){
        for(int i = 0; i < nx; i++){
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
    return get_value(vec2(x, y));
}
// float Terrain2D::height(float x, float y) const{
//     float i = (nx-1)*(x-min_p.x)/(max_p.x-min_p.x);
//     float j = (ny-1)*(y-min_p.y)/(max_p.y-min_p.y);
//     if(i < 0)
//         i = 0;
//     if(i > nx-1)
//         i = nx-1;
//     if(j < 0)
//         j = 0;
//     if(j > ny-1)
//         j = ny-1;
    
    
//     int i_0 = floor(i);
//     float s_i = i - i_0;
//     int j_0 = floor(j);
//     float s_j = j - j_0;


//     //top and right border case
//     if(s_i == 0 && s_j == 0)
//         return get_value(i_0, j_0);
//     if(s_i == 0)
//         return get_value(i_0, j_0)*(1-s_j)
//              + get_value(i_0, j_0+1)*s_j;
//     if(s_j == 0)
//         return get_value(i_0, j_0)*(1-s_i)
//              + get_value(i_0+1, j_0)*s_i;

//     return get_value(i_0, j_0)*(1-s_i)*(1-s_j)
//          + get_value(i_0, j_0+1)*(1-s_i)*s_j
//          + get_value(i_0+1, j_0)*s_i*(1-s_j)
//          + get_value(i_0+1, j_0+1)*s_i*s_j;
// }

Point Terrain2D::point(int i, int j) const{
    vec2 v = get_vec(i, j);
    return Point(v.x, v.y, height(i, j));
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

bool Terrain2D::is_water(float x, float y) const{
    return height(x, y) < 0;
}

void Terrain2D::add_city(vec2 position, int nb_crossroad, float crossroad_radius, float streat_size, int nb_direction){

    City new_city = City(this, position, crossroad_radius, streat_size, nb_direction);
    for(int _ = 1; _ < nb_crossroad; _++)
        new_city.add_random_crossroad();        

    cities.push_back(new_city);
}

void Terrain2D::compute_city_paths(float tolerence){
    for(size_t i = 0; i < cities.size(); i++)
        cities[i].compute_path(tolerence);
}

void Terrain2D::connect_cities(){
    for(size_t i = 0; i < paths.size(); i++){
        for(size_t j = 0; j < cities.size(); j++){
            std::vector<vec2> points = paths[i].get_points();
            if(cities[j].in_city(points[points.size()-1])){
                int last_point_in_city_id = points.size()-1;
                for(size_t k = points.size()-1; k < points.size(); k--)
                    if(cities[j].in_city(points[k]))
                        last_point_in_city_id = k;

                cities[j].add_entrence(points[last_point_in_city_id]);
                paths[i].cut_after(last_point_in_city_id);
                //TODO cut path at id last_point_in_city_id
            }
            points = paths[i].get_points();
            if(cities[j].in_city(points[0])){
                int last_point_in_city_id = 0;
                for(size_t k = 0; k < points.size(); k++)
                    if(cities[j].in_city(points[k]))
                        last_point_in_city_id = k;
                
                cities[j].add_entrence(points[last_point_in_city_id]);
                paths[i].cut_before(last_point_in_city_id);
                //TODO cut path at id last_point_in_city_id
            }
        } 
    }
}


void Terrain2D::comput_trees(const Forest &forest){
    trees = forest.get_trees(this);
}

std::vector<Transform> Terrain2D::get_tree_transform(TreeType type) const{
    std::vector<Transform> tree_transforms = std::vector<Transform>();
    std::uniform_real_distribution<float> u(0.0, 360);
    std::uniform_real_distribution<float> u01(0.0, 1.0);
    std::default_random_engine rng(42);
    for(Tree tree : trees)
        if(tree.type == type){
            float size = sqrt(sqrt(u01(rng)*tree.proba));
            if(size < 0.1)
                size = 0.1;
            tree_transforms.push_back(
                Translation(Vector(tree.position.x, tree.position.y, height(tree.position.x, tree.position.y)))
                *Scale(tree.radius*size, tree.radius*size, tree.radius*size)
                *RotationZ(u(rng)));
        }
            
    return tree_transforms;
}

void Terrain2D::compute_bvhs(){

    //Cities BVH
    std::vector<vec2> centers_cities = std::vector<vec2>();
    std::vector<vec2> centers_street = std::vector<vec2>();
    float city_radius = 0;
    float street_radius = 0;
    for(size_t i = 0; i < cities.size(); i++){
        city_radius = cities[i].get_crossroad_radius();
        std::vector<vec2> crossroads = cities[i].get_crossroad_centers();
        centers_cities.insert(
            centers_cities.end(),
            std::make_move_iterator(crossroads.begin()),
            std::make_move_iterator(crossroads.end())
            );
        std::vector<Path> city_paths = cities[i].get_paths();
        for(size_t j = 0; j < city_paths.size(); j++){
            street_radius = city_paths[j].get_path_size();

            std::vector<vec2> street_point = city_paths[j].get_points();
            centers_street.insert(
                centers_street.end(),
                std::make_move_iterator(street_point.begin()),
                std::make_move_iterator(street_point.end())
                );

        }
    }
    std::cout << "creating city bvh" << std::endl;
    std::vector<Object2D*> centers_cities_obj = std::vector<Object2D*>();
    for(vec2 c : centers_cities)
        centers_cities_obj.push_back(new Circle(c, city_radius));
    bvhs.push_back(BVH(centers_cities_obj));
    // bvhs.push_back(BVH(centers_cities, city_radius));

    std::cout << "creating street bvh" << std::endl;
    std::vector<Object2D*> centers_street_obj = std::vector<Object2D*>();
    for(vec2 c : centers_street)
        centers_street_obj.push_back(new Circle(c, street_radius));
    bvhs.push_back(BVH(centers_street_obj));
    // bvhs.push_back(BVH(centers_street, street_radius));
    std::cout << "end creating city bvh" << std::endl;

    std::vector<vec2> centers_road = std::vector<vec2>();
    float road_radius = 0;
    for(size_t i = 0; i < paths.size(); i++){
        road_radius = paths[i].get_path_size();
        std::vector<vec2> road_point = paths[i].get_points();
        centers_road.insert(
            centers_road.end(),
            std::make_move_iterator(road_point.begin()),
            std::make_move_iterator(road_point.end())
            );
    }
    std::cout << "creating road bvh" << std::endl;
    std::vector<Object2D*> centers_road_obj = std::vector<Object2D*>();
    for(vec2 c : centers_road)
        centers_road_obj.push_back(new Circle(c, road_radius));
    bvhs.push_back(BVH(centers_road_obj));
    // bvhs.push_back(BVH(centers_road, road_radius));


}

bool Terrain2D::intsersection_with_bvh(const vec2& p, float r) const{
    Object2D * obj = new Circle(p, r);
    for(size_t i = 0; i < bvhs.size(); i++)
        if(bvhs[i].intersection(obj))
            return true;
    return false;
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
    int max_iteration = 10000;
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

float Terrain2D::ambiant_occlusion(int i, int j, int nb_ray) const{
    float ambiant = 0;
    std::uniform_real_distribution<float> u01(0.0, 1.0);
    std::default_random_engine rng(42);

    for(int _ = 0; _ < nb_ray; _++){
        float r1 = u01(rng);
        float r2 = u01(rng);
        Vector n = normal(i, j);
        Vector dir(cos(2*M_PI*r1)*sqrt(1-r2*r2), sin(2*M_PI*r1)*sqrt(1-r2*r2), r2);
        dir = Rotation(Vector(0, 0, 1), n)(dir);
        //float proba_dir = 1/(2*M_PI);

        Point o = point(i, j) + 2*epsilon*n;

        if(!ray_intersection(o, dir))
            ambiant+=1.0/nb_ray;
    }
    return ambiant;
}

float Terrain2D::path_cost(vec2 start, vec2 end) const{
    float z = height(start.x, start.y) - height(end.x, end.y);
    float x = start.x - end.x;
    float y = start.y - end.y;
    std::pair<int, int> coord = get_coordinate(start);
    vec2 grad = gradient(coord.first, coord.second);
    float coef = 1;
    if(is_water(end.x, end.y))
        coef = 20;
    return coef*(sqrt(x*x + y*y + z*z)+5*abs(x*grad.x + y*grad.y));
    // return coef*(sqrt(x*x + y*y + 10*z*z)+abs(x*grad.x + y*grad.y));
}

adjacency_list_t Terrain2D::get_adjacency_list(int n, int scale, int min_x, int min_y, int max_x, int max_y) const{
    int m_nx = nx/scale;
    int m_ny = ny/scale;
    adjacency_list_t adjacency_list = std::vector<std::vector<neighbor>>(m_nx*m_ny);

    std::vector<std::pair<int, int>> mask = create_Mask_neighborhood(n);
    for(int i = 0; i < m_nx; i++){
        for(int j = 0; j < m_ny; j++){
            Point p = point(i*scale, j*scale);
            for(std::pair<int, int> m : mask){
                int i_ = m.first + i;
                int j_ = m.second + j;
                Point p_ = point(i_*scale, j_*scale);
                if(i_ >= min_x && i_ < max_x && j_ >= min_y && j_ < max_y){
                    float cost = path_cost(vec2(p.x, p.y), vec2(p_.x, p_.y));
                    adjacency_list[get_index(i, j, m_nx, m_ny)].push_back(neighbor(get_index(i_, j_, m_nx, m_ny), cost));
                }
            }
        }
    }
    return adjacency_list;
}

adjacency_list_t Terrain2D::get_adjacency_list(int n, int scale) const{
    return get_adjacency_list(n, scale, 0, 0, nx/scale, ny/scale);
}

void Terrain2D::draw_path(vec2 start, vec2 end, float road_size, int scale, int n){
    adjacency_list_t adj = get_adjacency_list(n, scale);
    int m_nx = nx/scale;
    int m_ny = ny/scale;
    int i = (m_nx-1)*(start.x-min_p.x)/(max_p.x-min_p.x);
    int j = (m_ny-1)*(start.y-min_p.y)/(max_p.y-min_p.y);
    int i_ = (m_nx-1)*(end.x-min_p.x)/(max_p.x-min_p.x);
    int j_ = (m_ny-1)*(end.y-min_p.y)/(max_p.y-min_p.y);
    vertex_t source = get_index(i, j, m_nx, m_ny);
    vertex_t dest = get_index(i_, j_, m_nx, m_ny);

    std::vector<weight_t> min_distance;
    std::vector<vertex_t> previous;

    DijkstraComputePaths(source, adj, min_distance, previous);
    std::list<vertex_t> path = DijkstraGetShortestPathTo(dest, previous);
    float ex = (max_p.x - min_p.x)/(nx-1);
    float ey = (max_p.y - min_p.y)/(ny-1);
    float r = road_size/std::min(ex, ey);

    std::vector<vec2> path_points;
    for(vertex_t v : path){
        std::pair<int, int> coord = get_coordinate(v, m_nx, m_ny);
        path_points.push_back(vec2(get_vec(coord.first*scale, coord.second*scale)));
    }
    paths.push_back(Path(path_points, road_size));
}

std::vector<Path> Terrain2D::get_network_path(std::vector<vec2> points, int min_x, int min_y, int max_x, int max_y, float road_size, float tolerence, int scale, int n){
    adjacency_list_t adj = get_adjacency_list(n, scale, min_x, min_y, max_x, max_y);

    std::vector<Path> m_paths;

    int m_nx = nx/scale;
    int m_ny = ny/scale;
    std::vector<std::vector<weight_t>> min_distance;
    std::vector<std::vector<vertex_t>> previous;
    std::vector<int> indexes;

    for(size_t k = 0; k < points.size(); k++){
        int i = (m_nx-1)*(points[k].x-min_p.x)/(max_p.x-min_p.x);
        int j = (m_ny-1)*(points[k].y-min_p.y)/(max_p.y-min_p.y);
        min_distance.push_back(std::vector<weight_t>());
        previous.push_back(std::vector<vertex_t>());
        indexes.push_back(get_index(i, j, m_nx, m_ny));
        DijkstraComputePaths(indexes[k], adj, min_distance[k], previous[k]);
    }
    std::vector<std::vector<weight_t>> network_distances = std::vector<std::vector<weight_t>>(points.size());
    std::vector<std::vector<bool>> keep_path = std::vector<std::vector<bool>>(points.size());
    //network_distances[i][j] : weight of the path i -> j
    for(size_t i = 0; i < points.size(); i++){
        for(size_t j = 0; j < points.size(); j++){
            network_distances[i].push_back(min_distance[i][indexes[j]]);
            keep_path[i].push_back(true);
        }
    }

    for(size_t i = 0; i < points.size(); i++){
        for(size_t j = 0; j < points.size(); j++){
            for(size_t k = 0; k < points.size(); k++){
                if(pow(network_distances[i][k], tolerence) + pow(network_distances[k][j], tolerence) < pow(network_distances[i][j], tolerence))
                    keep_path[i][j] = false;
            }
        }
    }

    //drawing
    for(size_t i = 0; i < points.size(); i++){
        for(size_t j = 0; j < points.size(); j++){
            if(!keep_path[i][j] || i <= j)
                continue;
            // std::cout << "keep path " << i << "(" << points[i].x << ", " << points[i].y << ")" << "->" << j << "(" << points[j].x << ", " << points[j].y << ")" << " of length : " << network_distances[i][j] << std::endl;
            std::list<vertex_t> path = DijkstraGetShortestPathTo(indexes[j], previous[i]);

            std::vector<vec2> path_points;
            
            for(vertex_t v : path){
                std::pair<int, int> coord = get_coordinate(v, m_nx, m_ny);
                path_points.push_back(vec2(get_vec(coord.first*scale, coord.second*scale)));
            }
            // path_points.push_back(points[i]);
            // path_points.push_back(points[j]);
            path_points[0] = points[i];
            path_points[path_points.size()-1] = points[j];
            m_paths.push_back(Path(path_points, road_size));
        }
    }
    return m_paths;
}

std::vector<Path> Terrain2D::get_network_path(std::vector<vec2> points, float road_size, float tolerence, int scale, int n){
    return get_network_path(points, 0, 0, nx/scale, ny/scale, road_size, tolerence, scale, n);
}

void Terrain2D::add_paths(const std::vector<Path>& m_paths){
    for(Path p : m_paths)
        paths.push_back(p);
}

void Terrain2D::refine(float precision){
    for(size_t i = 0; i < paths.size(); i++)
        paths[i].refine(precision);
}



void Terrain2D::apply_water(float water_level){
    for(int i = 0; i < nx; i++){
        for(int j = 0; j < ny; j++){
            if(get_value(i, j) < water_level){
                min_p.z = water_level;
                set_value(i, j, water_level);
            }
        }
    }
}

void Terrain2D::export_colored_terrain(const char *file, int scale) const{
    int m_nx = nx*scale;
    int m_ny = ny*scale;
    Image image(m_nx, m_ny, Color(0));

    for(int i = 0; i < nx; i++)
        for(int j = 0; j < ny; j++)
            for(int k = 0; k < scale; k++)
                for(int l = 0; l < scale; l++)
                    if(get_value(i, j) < 0)
                        image(i*scale + k, j*scale + l) = Color(0.15, 0.35, 0.75);
                    else if(slope(i, j) < 0.6)
                        image(i*scale + k, j*scale + l) = Color(0.2, 0.7, 0.2);
                    else
                        image(i*scale + k, j*scale + l) = Color(0.5, 0.5, 0.5);
    
    float res = std::min((max_p.x - min_p.x)/(m_nx-1), (max_p.y - min_p.y)/(m_ny-1));
    
    std::vector<Color> colors;
    colors.push_back(Color(0, 0, 0));
    colors.push_back(Color(0, 0, 1));
    colors.push_back(Color(0, 1, 0));
    colors.push_back(Color(0, 1, 1));
    colors.push_back(Color(1, 0, 0));
    colors.push_back(Color(1, 0, 1));
    colors.push_back(Color(1, 1, 0));
    colors.push_back(Color(1, 1, 1));

    // for(City city : cities){
    for(size_t i = 0; i < cities.size(); i++){
        City city = cities[i];
        std::cout << "city as " << city.get_paths().size() << " paths." << std::endl;
        int r = city.get_crossroad_radius()/res;
        for(vec2 crossroad : city.get_crossroad_centers()){
            if(is_water(crossroad.x, crossroad.y))
                std::cout << "CAREFOUR DANS L'EAU" << std::endl;
            std::pair<int, int> coord = get_coordinate(crossroad, m_nx, m_ny);
            for(int k = -r; k <= r; k++){
                for(int l = -r; l <= r; l++){
                    int k_ = k+coord.first;
                    int l_ = l+coord.second;
                    if(k*k + l*l <= r*r && k_ >= 0 && k_ < m_nx && l_ >= 0 && l_ < m_ny)
                        image(k_, l_) = colors[i%8]; 
                        // image(k_, l_) = Color(0.4, 0.4, 0.4); 
                }
            }
        }


        
        for(Path path : city.get_paths()){
            std::vector<vec2> points = path.get_points(res/2);
            int r = path.get_path_size()/res;
            for(vec2 point : points){
                std::pair<int, int> coord = get_coordinate(point, m_nx, m_ny);
                for(int k = -r; k <= r; k++){
                    for(int l = -r; l <= r; l++){
                        int k_ = k+coord.first;
                        int l_ = l+coord.second;
                        if(k*k + l*l <= r*r && k_ >= 0 && k_ < m_nx && l_ >= 0 && l_ < m_ny)
                            image(k_, l_) = Color(0.2, 0.2, 0.2); 
                    }
                }
            }
        }
    }
    for(Path path : paths){
        std::cout << "resolution : " << res/2 << std::endl;
        std::vector<vec2> points = path.get_points(res/2);
        int r = path.get_path_size()/res;
        for(vec2 point : points){
            std::pair<int, int> coord = get_coordinate(point, m_nx, m_ny);
            for(int k = -r; k <= r; k++){
                for(int l = -r; l <= r; l++){
                    int k_ = k+coord.first;
                    int l_ = l+coord.second;
                    if(k*k + l*l <= r*r && k_ >= 0 && k_ < m_nx && l_ >= 0 && l_ < m_ny)
                        image(k_, l_) = Color(0.8, 0.5, 0.3); 
                }
            }
        }
    }

    const bool draw_trees = false;
    if(draw_trees){
        for(Tree tree : trees){
            int r = tree.radius/res;
            std::pair<int, int> coord = get_coordinate(tree.position, m_nx, m_ny);
            for(int k = -r; k <= r; k++){
                for(int l = -r; l <= r; l++){
                    int k_ = k+coord.first;
                    int l_ = l+coord.second;
                    if(k*k + l*l <= r*r && k_ >= 0 && k_ < m_nx && l_ >= 0 && l_ < m_ny)
                        image(k_, l_) = Color(0.1, 0.5, 0.1); 
                }
            }
        }
    }
    
    write_image(image, file);
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
            indexes.push_back(get_index(i+0, j+0));
            indexes.push_back(get_index(i+1, j+1));
            indexes.push_back(get_index(i+0, j+1));

            indexes.push_back(get_index(i+0, j+0));
            indexes.push_back(get_index(i+1, j+0));
            indexes.push_back(get_index(i+1, j+1));
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

ScalarField2D Terrain2D::get_occlusions(int nb_ray){
    std::vector<float> ambiants = std::vector<float>(values.size());
    slope_max = max_slope();
    int lines = 0;
    std::cout << "[0/" << ny << "]" << std::flush;
    #pragma omp parallel for
    for(int j = 0; j < ny; j++){
        for(int i = 0; i < nx; i++)
            ambiants[get_index(i, j)] = ambiant_occlusion(i, j, nb_ray);
        std::cout << "\33[2K\r[" << ++lines << "/" << ny << "]" << std::flush;
    }
        
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