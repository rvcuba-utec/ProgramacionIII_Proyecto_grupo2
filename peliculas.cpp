#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

#define ROJO    "\033[38;5;196m"
#define BLANCO  "\033[38;5;231m"
#define AMARILLO "\033[38;5;220m"
#define GRIS    "\033[38;5;245m"
#define CYAN    "\033[38;5;87m"
#define VERDE   "\033[38;5;118m"
#define MAGENTA "\033[38;5;213m"
#define NARANJA "\033[38;5;208m"

#define BG_OSCURO "\033[48;5;234m"
#define BG_CARD   "\033[48;5;237m"

struct Pelicula {
    int id;
    string titulo;
    string año;
    string genero;
    string director;
    string sinopsis;
};

void limpiarPantalla() {
    cout << "\033[2J\033[H";
}

void linea(const string& color, char simbolo, int largo) {
    cout << color;
    for (int i = 0; i < largo; ++i) cout << simbolo;
    cout << RESET << "\n";
}

void cabecera() {
    cout << "\n";
    linea(ROJO, '=', 58);
    cout << BOLD << ROJO << "  ██████╗ " << RESET;
    cout << BOLD << BLANCO << "CINE" << RESET;
    cout << BOLD << ROJO << " ██████╗\n" << RESET;
    cout << BOLD << AMARILLO << "       Top 5 Películas Peruanas" << RESET << "\n";
    linea(ROJO, '=', 58);
    cout << "\n";
}

void mostrarPelicula(const Pelicula& p) {
    cout << BG_CARD;
    cout << "  " << BOLD << AMARILLO << "▶  #" << p.id << "  " << RESET;
    cout << BOLD << BLANCO << p.titulo << RESET << "\n";
    cout << RESET;

    cout << "     " << CYAN  << p.año
              << GRIS   << "  │  "
              << VERDE  << p.genero
              << GRIS   << "  │  "
              << MAGENTA<< p.director
              << RESET  << "\n";

    cout << "     " << DIM << GRIS << p.sinopsis << RESET << "\n";
    linea(GRIS, '-', 58);
}

int main() {
    limpiarPantalla();

    vector<Pelicula> peliculas = {
        {1,
         "La Paisana Jacinta",
         "1999",
         "Comedia",
         "Jorge Benavides",
         "Jacinta llega a Lima desde los Andes buscando trabajo\n     y se mete en enredos hilarantes por la ciudad."},

        {2,
         "El Cuy del Terror",
         "2018",
         "Terror / Comedia",
         "Gustavo Casanova",
         "Un cuy maldito siembra el caos en un barrio limeño.\n     Nadie puede dormir tranquilo con ese roedor suelto."},

        {3,
         "Chicha tu Mare",
         "2013",
         "Comedia",
         "Frank Pérez-Garland",
         "Dos mundos opuestos de Lima colisionan cuando\n     sus hijos se enamoran contra toda lógica social."},

        {4,
         "Contracorriente",
         "2009",
         "Drama",
         "Javier Fuentes-León",
         "En un pueblo pesquero del Pacífico, un hombre lucha\n     entre el secreto de su amor y las tradiciones del mar."},

        {5,
         "Cementerio General",
         "2013",
         "Terror",
         "Dorian Fernández-Moris",
         "Un grupo de jóvenes graba en el cementerio más\n     antiguo de Lima y despierta lo que no debía despertar."}
    };

    cabecera();

    for (const auto& p : peliculas) {
        mostrarPelicula(p);
    }

    cout << "\n" << BOLD << AMARILLO << peliculas.size()
              << " películas mostradas. ¡A disfrutar!" << RESET << "\n\n";

    return 0;
}
