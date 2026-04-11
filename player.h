#ifndef PLAYER_H
#define PLAYER_H
# include <vector>
#include <QString>

// Hier kommen die Variablen und Funktionen die einen Spieler definieren
// ein Dictionary was alles enthählt über das auslegen eines Kartenblocks
struct lay_out_colour {
    std::vector<QString> path_cards;   // Pfad für die Karte
    QString number;         // Zahl des Kartenblocks
    int amount;             // Anzahl der normalen Karten
    int amount_small_joker; // Anzahl der kleinen Joker
    int amount_big_joker;   // Anzahl der großen Joker
};

struct possbile_layout_results{
    std::vector<QString> possible_layout_cards; // Vektor mit den Karte die Spieler auslegen kann
    std::vector<int> sum_cardsblocks;        // Vektor mit den Summen der Zahlenblöcke
    std::vector<int> amount_lay_out;         // Vektor mit der Anzahl von Karten in einem Zahlenblock
    //std::vector<std::vector<int>> need_joker;          // Vektor mit Anzahl der Joker die man braucht fürs auslegen eines Zahlenblocks
};

class PLAYER
{
private:
    int total_score = 0;    // Punktzahl des Spieler
public:
    // Variablen
    QString name;
    std::vector<QString> cards;	// Karten des Spielers

    // könnte vielleicht einen Vektor erstellen der [[Zahl, Farbe], ...] Muster hat anstatt ["Zahl Farbe", ...]

    std::vector<int> cards_value;	// Wert der Karten des Spielers
    int number_redthree = 0;		// Anzahl der roten dreien
    std::vector<int> amount;		// Anzahl der Karten in einem Zahlenblock
    std::vector<int> amount_value;	// Wert der Karten in einem Zahlenblock
    int min_value = 50;				// minimale Wert mit dem rausgekommen werden muss
    bool be_outside = false;
    bool round_finished = false;
    possbile_layout_results possible_lay_out;

    //da raus ein erde machen
    std::vector<lay_out_colour> lay_out;
    std::vector<lay_out_colour> street_pile;

    // Funktionen
    void red_three(std::vector<QString>& pile, PLAYER& player);				// Funktion zum finden der roten dreien
};

#endif // PLAYER_H
