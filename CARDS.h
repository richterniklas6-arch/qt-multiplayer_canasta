#ifndef CARDS_H
#define CARDS_H
#include <vector>
#include <string>
#include <QString>


class CARDS {
public:
    // Diese Klasse enthält die Variablen und Funktionen
    std::vector<QString> color = { "of_clubs", "of_spades", "of_hearts", "of_diamonds" };
    std::vector<QString> number = { "2_", "3_", "4_", "5_", "6_", "7_", "8_", "9_", "10_", "jack_", "queen_", "king_", "ace_", "joker" };
    std::vector<QString> order = { "3", "4", "5", "6", "7", "8", "9", "10", "jack", "queen", "king", "ace", "2", "joker" };
    std::vector<int> value; // wert der Karte, in reihenfolge von number

    std::vector<QString> pile; // Stapel
    std::vector<int> pile_value;  // Stapel Wert
    std::vector<QString> draw_pile;  // Ablagestapel
    std::vector<int> draw_pile_value;     // Ablagestapel Wert

    // in konstruktur könnte Karten erstellen rein und Wert festlegen
    CARDS(); // Konstruktur der Wert der Karten definiert
};

#endif // CARDS_H
