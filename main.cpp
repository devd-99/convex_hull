////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <complex>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;
////////////////////////////////////////////////////////////////////////////////

// init complex nums as Point p(x, y)
typedef std::complex<double> Point;

// init polygon as vector of points
typedef std::vector<Point> Polygon;

// function to compute determinant (neg Deg = CW, pos Deg = CCW)
double inline det(const Point &u, const Point &v) {
	return u.real() * v.imag() - u.imag() * v.real();
}


struct Compare {
	Point p0; // Leftmost point of the poly
	bool operator ()(const Point &p1, const Point &p2) {
		
		double orientation = det(p1 - p0, p2 - p0);
		
		if (orientation == 0) return abs(p1 - p0) < abs(p2 - p0);  // If collinear, sort by distance.
		
		return orientation > 0;  // Return true if p1 is counterclockwise to p2 w.r.t. p0.
	}

};

// function to compute salient angle
bool inline salientAngle(Point &a, Point &b, Point &c) { 
	
	return (det(b-a, c-a) > 0);

}

////////////////////////////////////////////////////////////////////////////////

Polygon convex_hull(std::vector<Point> &points) {
    if (points.size() < 3) return points; // Can't form a hull with less than 3 points
    
    // 1. Find the point with the lowest y-coordinate
    auto p0_it = std::min_element(points.begin(), points.end(), 
        [](const Point &a, const Point &b) {
            return (a.imag() < b.imag()) || (a.imag() == b.imag() && a.real() < b.real());
        });
    std::swap(points[0], *p0_it);
    Point p0 = points[0];

    // 2. Sort the points based on their polar angle with respect to p0
    Compare order;
    order.p0 = p0;
    std::sort(points.begin() + 1, points.end(), order); // We can skip the first element

    // 3. Build the convex hull
    Polygon hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);
    hull.push_back(points[2]);

    for (size_t i = 3; i < points.size(); i++) {
        while (hull.size() > 1 && !salientAngle(*(hull.end() - 2), hull.back(), points[i])) {
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }

    return hull;
}


////////////////////////////////////////////////////////////////////////////////

std::vector<Point> load_xyz(const std::string &filename) {
	std::vector<Point> points;
	std::ifstream file(filename);
	if (!file.is_open()) {
		// TODO: throw exception
		throw std::runtime_error(" Error! Failed to open file " + filename);
	}
	int n;
	file >> n;
	double x, y, z; 
	for (int i = 0; i < n; ++i) {

		// read x, y, z from file
		file >> x >> y >> z;

		// since we are in 2D, we ignore z.
		points.emplace_back(x, y);

	}

	return points;
}


void save_obj(const std::string &filename, Polygon &poly) {

	cout << "Saving to file " << filename << endl;

	std::ofstream out(filename);
	if (!out.is_open()) {
		throw std::runtime_error("failed to open file " + filename);
	}
	out << std::fixed;
	for (const auto &v : poly) {
		out << "v " << v.real() << ' ' << v.imag() << " 0\n";
	}
	for (size_t i = 0; i < poly.size(); ++i) {
		out << "l " << i+1 << ' ' << 1+(i+1)%poly.size() << "\n";
	}
	out << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char * argv[]) {
	if (argc <= 2) {
		std::cerr << "Usage: " << argv[0] << " points.xyz output.obj" << std::endl;
	}
	cout << "Hello World" << endl;
	// load points from file
	std::vector<Point> points = load_xyz(argv[1]);

	// compute convex hull
	Polygon hull = convex_hull(points);

	// save hull to file
	save_obj(argv[2], hull);

	return 0;
}
