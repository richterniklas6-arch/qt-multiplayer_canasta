#include "../Canasta_3_0/CARDS.h"
# include <algorithm>
# include <random>

CARDS::CARDS() {
    // Konstruktur der Wert der Karten definiert
    int w;
    for (int n = 0; n < number.size(); n++) {
        if (number[n] == "joker_") { w = 50; }
        else if (number[n] == "ace_" || number[n] == "2_") { w = 20; }
        else if (number[n] == "6_" || number[n] == "5_" || number[n] == "4_" || number[n] == "3_") {
            w = 5;
        }
        else { w = 10; } //Karten K bis 7 wert auf 10
        value.push_back(w);
    }

    // Karten erschaffen und mischen
    for (int doppelt = 0; doppelt < 2; doppelt++) {     // doppltes Kartendeck
        for (int zahl = 0; zahl < 14; zahl++) {			// alle 14 Zahlen durchgehen
            for (int farbe = 0; farbe < 4; farbe++) {	// alle 4 Farben durchgehen
                if(number[zahl] == "joker"){
                    pile.push_back(number[zahl]);
                }
                else{pile.push_back(number[zahl] + color[farbe]);}
            }
        }
    }
    //=> vollständiges Kartendeckvektor (in Reihenfolge)

    // Karten mischen
    std::random_device rd; // zufällige Zahlgenerator (Seed für mt19937)
    std::mt19937 g(rd());	// erzeugt viele Zufallszahlen
    std::shuffle(pile.begin(), pile.end(), g);	// shuffle läuft kartendeck von .begin() bis .end()
                                                // gibt jedem element eine zufallsposition die mit g() erzeugt wurde
    //=> vollständiges Kartendeck (gemischt)
}
