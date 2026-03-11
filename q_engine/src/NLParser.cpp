#include "NLParser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace q_engine {

bool NLParser::extract_value_and_unit(const std::string& word, double& value, std::string& unit) {
    size_t i = 0;
    while (i < word.length() && (std::isdigit(word[i]) || word[i] == '.' || word[i] == '-')) {
        i++;
    }
    
    if (i == 0) return false; // Pas de nombre trouvé
    
    try {
        value = std::stod(word.substr(0, i));
        unit = word.substr(i);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<PhysicalEntity> NLParser::parse_hypothesis(const std::string& hypothesis) {
    std::vector<PhysicalEntity> entities;
    std::istringstream stream(hypothesis);
    std::string word;
    
    // Un parseur lexical simple (regex-like) pour "L'Oracle" hors LLM externe
    while (stream >> word) {
        // Nettoyage basique (retirer ponctuation de fin)
        if (!word.empty() && std::ispunct(word.back())) {
            word.pop_back();
        }
        
        double val;
        std::string unit;
        if (extract_value_and_unit(word, val, unit)) {
            // Identifier le type en fonction de l'unité ou du contexte
            std::string type = "unknown";
            if (unit == "G" || unit == "g") type = "pressure";
            else if (unit == "m" || unit == "km") type = "distance";
            else if (unit == "K" || unit == "C") type = "temperature";
            else if (unit == "dB") type = "noise";
            
            // Convertions basiques
            if (unit == "km") { val *= 1000.0; unit = "m"; }
            
            entities.push_back({type, val, unit});
        }
    }
    
    return entities;
}

} // namespace q_engine
