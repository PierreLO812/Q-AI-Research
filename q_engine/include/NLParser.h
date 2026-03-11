#ifndef NL_PARSER_H
#define NL_PARSER_H

#include <string>
#include <map>
#include <vector>

namespace q_engine {

// Représente une entité physique extraite de la phrase (NLP)
struct PhysicalEntity {
    std::string type;   // ex: "pressure", "distance", "temperature"
    double value;       // ex: 2.0, 100.0, 300.0
    std::string unit;   // ex: "G", "m", "K"
};

class NLParser {
public:
    // Parse une phrase complexe en langage naturel pour extraire les entités physiques
    // Exemple : "est ce que le signal survit à 2G de pression sur 100m de fibre ?"
    static std::vector<PhysicalEntity> parse_hypothesis(const std::string& hypothesis);
    
private:
    static bool extract_value_and_unit(const std::string& word, double& value, std::string& unit);
};

} // namespace q_engine

#endif
