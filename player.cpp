#include "PLAYER.h"
#include "PRINT_ORDER.h"
#include <vector>
#include <QDebug>

void PLAYER::red_three(std::vector<QString>& pile, PLAYER& player) {
    for (int i = 0; i < cards.size(); i++) { // geht Karten des Spielers durch
        if (cards[i] == "3_of_hearts" || cards[i] == "3_of_diamonds") { // wenn eine rote drei
            number_redthree++;
            qDebug() << "Glückwunsch du hast eine rote drei gefunden!";

            cards.erase(cards.begin() + i); // lösche aus Spieler Karten die derzeitige Karte
            i--; // eins zurück, da eine Karte gelöscht wurde

            if (!pile.empty()) { // Karte nachziehen
                qDebug() << "Du hast diese Karte nachgezogen: " << pile[0];
                cards.push_back(pile[0]);  // fügt erste Karte des Nachziehstapel zu Spieler Karten hinzu
                pile.erase(pile.begin());  // löscht die erste Karte des Nachziehstapel
            }
        }
    }
}

//void PLAYER::total_score_after_round(auto straßen_haufen){
//    // nach einer Runde werden die Punkte die erziehlt wurden
//
//}
