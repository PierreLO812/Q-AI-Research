#include "SchemaGenerator.h"
#include <sstream>
#include <cmath>

namespace q_engine {

std::string SchemaGenerator::generate_equation_svg(ExprPtr expr) {
    if (!expr) return "";
    
    // In a real application, this would recursively compute bounding boxes for the AST elements.
    // For our C++ engine demo, we generate a sleek text-based SVG presentation tile.
    std::string eq = expr->to_string();
    
    std::ostringstream svg;
    svg << "<svg width=\"400\" height=\"100\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    svg << "  <rect width=\"100%\" height=\"100%\" fill=\"#1E1E1E\" rx=\"10\"/>\n";
    svg << "  <text x=\"50%\" y=\"50%\" font-family=\"monospace\" font-size=\"20\" fill=\"#00FFCC\" text-anchor=\"middle\" alignment-baseline=\"middle\">\n";
    svg << "    " << eq << "\n";
    svg << "  </text>\n";
    svg << "</svg>\n";
    
    return svg.str();
}

std::string SchemaGenerator::generate_bloch_sphere_svg(double prob_0, double prob_1) {
    // Generate an illustrative 2D projection of the Bloch sphere
    // z = prob_0 - prob_1
    // (We assume phase is 0 for this simple 2D representation demo)
    double z = prob_0 - prob_1;
    double x = std::sqrt(1.0 - z*z);
    
    // Map to SVG coordinates (center 100,100, radius 80)
    double cx = 100;
    double cy = 100;
    double r = 80;
    
    double px = cx + x * r;
    double py = cy - z * r; // Minus because SVG y goes down
    
    std::ostringstream svg;
    svg << "<svg width=\"200\" height=\"200\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    // Background and Circle
    svg << "  <rect width=\"100%\" height=\"100%\" fill=\"#121212\" rx=\"10\"/>\n";
    svg << "  <circle cx=\"100\" cy=\"100\" r=\"80\" stroke=\"#444\" stroke-width=\"2\" fill=\"none\"/>\n";
    svg << "  <ellipse cx=\"100\" cy=\"100\" rx=\"80\" ry=\"20\" stroke=\"#333\" stroke-width=\"1\" fill=\"none\" stroke-dasharray=\"4\"/>\n";
    
    // Axes
    svg << "  <line x1=\"100\" y1=\"10\" x2=\"100\" y2=\"190\" stroke=\"#555\" stroke-width=\"1\"/>\n";
    svg << "  <line x1=\"10\" y1=\"100\" x2=\"190\" y2=\"100\" stroke=\"#555\" stroke-width=\"1\"/>\n";
    svg << "  <text x=\"105\" y=\"15\" fill=\"#888\" font-size=\"10\">|0></text>\n";
    svg << "  <text x=\"105\" y=\"195\" fill=\"#888\" font-size=\"10\">|1></text>\n";
    
    // State Vector
    svg << "  <line x1=\"100\" y1=\"100\" x2=\"" << px << "\" y2=\"" << py << "\" stroke=\"#00FFCC\" stroke-width=\"3\"/>\n";
    svg << "  <circle cx=\"" << px << "\" cy=\"" << py << "\" r=\"4\" fill=\"#00FFCC\"/>\n";
    svg << "</svg>\n";
    
    return svg.str();
}

} // namespace q_engine
