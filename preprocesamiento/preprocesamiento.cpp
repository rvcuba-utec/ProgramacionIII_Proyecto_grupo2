#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

// CONFIGURACIÓN
const std::string CSV_PATH = "../data/wiki_movie_plots_deduped.csv";

// ESTRUCTURA DE UNA PELÍCULA
struct Movie {
    int id;
    std::string title;
    std::string synopsis;
    std::string director;
    std::string cast;
    std::string genre;
    std::string releaseYear;
    std::string titleNormalized;
};

// MAPA DE CARACTERES
std::string removeAccents(const std::string& str) {
    static const std::unordered_map<std::string, std::string> utf8_map = {
        {"é","e"},{"á","a"},{"í","i"},{"ó","o"},{"ú","u"},
        {"è","e"},{"à","a"},{"ì","i"},{"ò","o"},{"ù","u"},
        {"ë","e"},{"ä","a"},{"ï","i"},{"ö","o"},{"ü","u"},
        {"ê","e"},{"â","a"},{"î","i"},{"ô","o"},{"û","u"},
        {"ñ","n"},{"ç","c"},{"ā","a"},{"ē","e"},{"ī","i"},{"ō","o"},{"ū","u"},
        {"İ","i"},{"ı","i"},{"æ","ae"},{"œ","oe"},{"ø","o"},{"ß","ss"},
        {"É","e"},{"Á","a"},{"Í","i"},{"Ó","o"},{"Ú","u"},{"Ñ","n"},{"Ç","c"},
        {"ś","s"},{"ć","c"},{"ž","z"},{"ł","l"},{"ń","n"},{"ñ","n"},

    };
    std::string result;
    result.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        if (c >= 0x80) {
            bool found = false;
            if (i + 1 < str.size()) {
                std::string utf8_char = str.substr(i, 2);
                auto it = utf8_map.find(utf8_char);
                if (it != utf8_map.end()) {
                    result += it->second;
                    i++;
                    found = true;
                }
            }
            if (!found) {
                if ((c & 0xF0) == 0xE0) i += 2;
                else if ((c & 0xF8) == 0xF0) i += 3;
                else if ((c & 0xE0) == 0xC0) i += 1;
                if (!result.empty() && result.back() != ' ') result += ' ';
            }
        } else {
            result += c;
        }
    }
    return result;
}

std::string normalizeText(const std::string& raw) {
    std::string processed = removeAccents(raw);
    std::string clean;
    clean.reserve(processed.size());
    for (char c : processed) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalnum(uc)) clean += std::tolower(uc);
        else if (std::isspace(uc)) clean += ' ';
        else clean += ' ';
    }
    std::string final;
    bool inSpace = false;
    for (char c : clean) {
        if (c == ' ') {
            if (!inSpace) { final += ' '; inSpace = true; }
        } else {
            final += c; inSpace = false;
        }
    }
    size_t start = final.find_first_not_of(' ');
    if (start == std::string::npos) return "";
    size_t end = final.find_last_not_of(' ');
    return final.substr(start, end - start + 1);
}

std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::istringstream iss(text);
    std::string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}

std::vector<std::string> parseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    current += '"'; ++i;
                } else {
                    inQuotes = false;
                }
            } else current += c;
        } else {
            if (c == '"') inQuotes = true;
            else if (c == ',') { fields.push_back(current); current.clear(); }
            else current += c;
        }
    }
    fields.push_back(current);
    return fields;
}

// CARGA Y PRE‑PROCESAMIENTO
std::vector<Movie> loadAndCleanMovies() {
    std::ifstream file(CSV_PATH);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir " << CSV_PATH << std::endl;
        return {};
    }
    std::string header;
    std::getline(file, header);
    std::vector<Movie> movies;
    int movieId = 1;
    std::string record;
    int quoteCount = 0;
    std::string line;
    while (std::getline(file, line)) {
        for (char c : line) if (c == '"') quoteCount++;
        if (!record.empty()) record += '\n';
        record += line;
        if (quoteCount % 2 == 0) {
            auto fields = parseCsvLine(record);
            record.clear();
            quoteCount = 0;
            if (fields.size() < 8) continue;
            Movie m;
            m.id = movieId++;
            m.title = fields[1];
            m.director = fields[3];
            m.cast = fields[4];
            m.genre = fields[5];
            m.synopsis = fields[7];
            m.releaseYear = fields[0];
            if (m.title.empty() || m.title.find_first_not_of(" ") == std::string::npos) continue;
            m.titleNormalized = normalizeText(m.title);
            movies.push_back(std::move(m));
        }
    }
    return movies;
}

// MAIN
int main() {
    std::cout << "Cargando películas..." << std::endl;
    std::vector<Movie> movies = loadAndCleanMovies();

    std::cout << "\nTotal de películas cargadas: " << movies.size() << std::endl;

    size_t numMuestras = std::min<size_t>(5, movies.size());
    std::cout << "\n--- Primeras " << numMuestras << " películas ---\n";
    for (size_t i = 0; i < numMuestras; ++i) {
        std::cout << "ID: " << movies[i].id << "\n";
        std::cout << "  Título original: " << movies[i].title << "\n";
        std::cout << "  Título normalizado: " << movies[i].titleNormalized << "\n";
        std::cout << "  Año: " << movies[i].releaseYear << "\n";
        std::cout << "  Director: " << movies[i].director << "\n";
        std::cout << "  Género: " << movies[i].genre << "\n\n";
    }

    return 0;
}
