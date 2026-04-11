/*#include "ROUND.h"
#include "PRINT_ORDER.h"
#include "CARDS.h"
#include "BACKEND.h"

#include <QDebug>
# include <vector>
# include <algorithm>
# include <map>
# include <string>
#include<iostream>
# include <algorithm>
# include <random>
# include <string>



ROUND::ROUND() {}

// Karten definieren und mischen und austeilen
void ROUND::deal_out_cards(PLAYER& p1,
                      PLAYER& p2,
                      CARDS& cards) {
    // Karten geben
    for (int i = 0; i < 14; i++) { // für jeden Spieler 14 Karten geben
        p1.cards.push_back(cards.pile[0]);	// Spieler 1
        cards.pile.erase(cards.pile.begin()); // entfernen der Karte vom stapel
        p2.cards.push_back(cards.pile[0]);	// Spieler 2
        cards.pile.erase(cards.pile.begin()); // entfernen der Karte vom Stapel
    }
    p1.red_three(cards.pile, p1);
    p2.red_three(cards.pile, p2);
    // erste Karte für Auslegestapel
    cards.draw_pile.push_back(cards.pile[0]);		// erste Karte Ablagestapel
    cards.pile.erase(cards.pile.begin());   // entfernen der Karte vom Stapel
    //=> jeder Spieler nun 14 Karten, es ist eine Karte aufgedeckt und die restlichen sind der stapel
    //Karten sortieren
    sortieren(p1, cards);
    sortieren(p2, cards);
}

// sortieren der Karten
void ROUND::sortieren(PLAYER& player, CARDS& k) {
    // vor sortieren die Blöcke leer machen
    player.amount.clear();
    player.amount_value.clear();

    // hier schauen was passier, ChatGPT fragen (sortieren der Karten des Spielers)
    std::map<QString, int> wertIndex;
    for (size_t i = 0; i < k.order.size(); ++i) {
        wertIndex[k.order[i]] = i;
    }
    std::sort(player.cards.begin(), player.cards.end(), [&](const QString& a, const QString& b) {
        QString wertA = a.section('_', 0, 0);
        QString wertB = b.section('_', 0, 0);
        return wertIndex[wertA] < wertIndex[wertB];
    });

    // sortieren des Stapels
    std::map<QString, int> wertIndex_pile;
    for (size_t i = 0; i < k.order.size(); ++i) {
        wertIndex_pile[k.order[i]] = i;
    }
    std::sort(k.pile.begin(), k.pile.end(), [&](const QString& a, const QString& b) {
        QString wertA = a.section('_', 0, 0);
        QString wertB = b.section('_', 0, 0);
        return wertIndex_pile[wertA] < wertIndex_pile[wertB];
    });

    // setzen wie viele Karten in einem Zahlenblock drin sind
    for (int i = 0; i < k.order.size(); i++) { // nach reihenfolge durchgehen
        int number = 0; // Anzahl der Karten in einem Block

        for (int j = 0; j < player.cards.size(); j++) {//alle Karten des Spielers durchgehen
            // string bauen der die zahl der Karte enthält
            QString wert = player.cards[j].section('_', 0, 0);

            if (wert == k.order[i]) {
                number++;// wenn derzeitige Karte dran ist in reihenfolge, erhöhe amount um 1
            }
        }
        player.amount.push_back(number); // aktuelliesere die amount der Karten in einem Zahlenblock
    }

    // Wert eines Kartenblockes berechnen
    player.amount_value = player.amount; //

    for (int i = 0; i < player.amount_value.size(); i++) {// jeden Zahlenblock durchgehen
        int multiplikator;
        if (i >= 0 && i < 3) { multiplikator = 5; }			// Karten 3 bis 6
        else if (i >= 3 && i < 11) { multiplikator = 10; }	// Karten 7 bis K
        else if (i >= 11 && i < 13) { multiplikator = 20; }	// A
        else if (i == 13) { multiplikator = 50; }	// großer Jocker
        else { multiplikator = 1; }							// für Jocker
        player.amount_value[i] = multiplikator * player.amount_value[i];// berechnen des Wertes eines Zahlenblocks
    }
}


// Spieler führt seinen Zug aus
void ROUND::take_card_pile(PLAYER& player, CARDS& cards ) {
    // Karte nehmen oder Haufen nehmen
    bool draw_pile_take_able = 0;   // zeigt an ob Haufen nehmbar ist
    int position_first_cdp;         // Position der Zahl der obersten Karte vom Ablagestapel vom Vektor "number"
    QString number_first_cdp = cards.draw_pile[0].section('_', 0, 0); // Zahl_ der oberste Karte des Nachziehstapels

    if (number_first_cdp != "joker"){ // da Joker in cards.number kein _ hat
        number_first_cdp += "_";
    }

    auto it = std::find(cards.number.begin(), cards.number.end(), number_first_cdp); // it zeigt auf element number_first_cdp im Vektor cards.number
    if (it != cards.number.end() ) { // wenn it außerhalb von cards.number zeigt
        position_first_cdp = it - cards.number.begin() - 1; // gibt die Position im cards.number von der ersten Karten des Ablagestapels aus
        qDebug() << "Die Position der obersten Karte des Ablagestapel im Vektor cards.number:" << position_first_cdp;
    }
    else { // Fehler abfangen
        qDebug() << "Nicht gefunden! Gesucht war:"<< number_first_cdp;
        }

    // Kann der Spieler nehmen? Mehre Möglichkeiten...
        // 1. Alles wird abgefangen wenn nur eine Karte auf Ablegestapel da oder wenn gesperrt ist
    if (cards.draw_pile.size() > 1 && number_first_cdp!="3") {
        // ... wenn Spieler mindestens 2 passende Karte in seiner Hand hat ***
        if (player.amount[position_first_cdp] > 2 ||               //*** 2 gleiche Karten oder
                (player.amount[position_first_cdp] > 1 &&          //*** 1 Karte und ein/mehrere Joker
                 player.amount[position_first_cdp] + player.amount[12] + player.amount[13] > 2)) {
            draw_pile_take_able = true; // stapel nun nehmbar
            qDebug() << "Der Spieler kann den Haufen nehmen, weil er die passenden Karten auf der Hand hat";
            //=> Spieler kann nehmen
        }
        // ... wenn karten draußen hat und nehmbar ist ***
        for (int outside_cards = 0; outside_cards < player.lay_out.size(); outside_cards++) { // geht den Vektor player.lay_out durch
            if (player.lay_out[outside_cards].number == number_first_cdp) { // wenn man die Karten schon ausgelegt hat
                draw_pile_take_able = true;
                qDebug() << "Der Spieler kann den Haufen nehmen, weil er die Karten draußen hat.";
                //=> Spieler kann nehmen
            }
        }
    }
    //=> entschieden ob der Spieler den Haufen nehmen kann (Ja oder Nein)

    // Frage ob der Spieler den Haufen nehmen will
    if (player.be_outside && draw_pile_take_able) { // Spieler muss draußen und Stapel nehmbar sein
        // das muss ins infopanel rein



        std::cout << "Willst du den Haufen nehmen? (Ja/Nein)" << std::endl;
        std::string antwort;
        std::cin >> antwort; // Antwort vom Spieler

        // Der Spieler nimmt den Karten Haufen
        if (antwort == "Ja") {
            std::vector<QString> neue_karten; // temporärer Vektor
            for (auto it = cards.draw_pile.begin(); it != cards.draw_pile.end(); ++it) {// Pointer der immer auf ein Element vom Ablagestapel zeigt
                if (*it == "3 Eiche" || *it == "3 Blatt") {
                    qDebug() << "Eine schwarze drei blockt die restlichen Karten";
                    break; // Schleife stoppen, schwarze drei gefunden
                }
                neue_karten.push_back(*it); // Karte aufnehmen
            }

            for (auto& karte : neue_karten) {
                player.cards.push_back(karte);                  // Aufnehmen der Karten in die Hand
                cards.draw_pile.erase(cards.draw_pile.begin()); // Gleichzeitig aus Ablagestapel löschen
            }
        }
    }
    // => der Spieler nimmt den Haufen genommen

    // Wenn der Spieler den Haufen nicht nehmen will oder kann

    else {
        qDebug() << "The player has/need to take one card:" << cards.pile[0];
        player.cards.push_back(cards.pile[0]); // fügt erste Karte des Nachziehstapel zu Spieler Karten hinzu
        cards.pile.erase(cards.pile.begin());  // löscht die erste Karte des Nachziehstapel
        player.red_three(cards.pile, player);   // schaut ob gezogen Karte eine rote drei ist
    }

    sortieren(player, cards);   // sortiert die Karten des Spieler und vergibt neue Anzahl der Nummerblöcke 
    //=> der Spieler hat eine Karte aufgenommen oder hat den Stapel genommen. Sein Deck wurde neu sortiert und gezählt
    bool spiel_fertig;
    spiel_fertig = true; //auslegen_definition(player, cards);
}


void ROUND::possible_lay_out_cards(PLAYER& player, CARDS& cards, Backend* backend) {
    // leeren der alten auslege Möglichkeiten
    player.possible_lay_out.possible_layout_cards.clear();
    player.possible_lay_out.sum_cardsblocks.clear();
    player.possible_lay_out.amount_lay_out.clear();

    std::vector<QString> possible_layout_cards; // Vektor mit den Karte die Spieler auslegen kann
    std::vector<int> sum_cardsblocks;        // Vektor mit den Summen der Zahlenblöcke
    std::vector<int> amount_lay_out;         // Vektor mit der Anzahl von Karten in einem Zahlenblock

    int min_value;                  // wieviele Punkte muss man zum auslegen haben

    int min_amount_card = 3;        // mindest Anzahl der Karten fürs auslegen
    qDebug() << "kann der spieler rauskomme?" << player.be_outside;
// Mindestpunktzahl zum auslegen
    if (player.be_outside == 1) { //wenn der Spieler draußen braucht er keine mindestrauslegen
        min_value = 0;
        min_amount_card = 2;
    }
    else {
        min_value = player.min_value; // wenn der Spieler noch nicht draußen ist, muss Minimum erreichen
    }
    //=> Minimum der Punktzahl festgelegt

//welche Karten kann der Spieler auslegen
    for (int i = 0; i < player.amount.size(); i++) {

        bool need_small_joker = true; // Aussage ob man sie Schleife mit den kleinen Joker durchgehen muss
        // wenn Joker auslegen kann
        if (    (i == 12 || i == 13) &&                                          // nur Joker dieser Block
                 player.amount[12] + player.amount[13] >= 3 &&                    // + nur wenn mehr als 3 Joker
                 player.amount_value[12] + player.amount_value[13] >= min_value) {// + nur wenn die Summe der Werte der Joker groß genug ist
            player.possible_lay_out.possible_layout_cards.push_back("Joker"); // nur die Zahl wird gespeichert, nicht die Farbe
            player.possible_lay_out.amount_lay_out.push_back(0); // da man mit Spin Box Anzahl festlegt
            player.possible_lay_out.sum_cardsblocks.push_back(0); // mit Spinbox legt man Wert fest
            i++; // überspringt fetten Joker wenn kleine und fette Joker hat
            continue;
        }
        // wenn Zahlkarte drei Stück hat, aber keine 3


        // schauen ob die beide else if zusammenfassen kann, da man min_amount_cards abfragen kann



        else if (player.amount[i] >= 3 &&		// nur wenn gleich/mehr als drei Karten
                 cards.order[i] != "3" &&		// nur wenn keine drei ist
                 player.amount_value[i] + player.amount_value[12] + player.amount_value[13] >= min_value) {	// nur wenn Summe der Karten+Joker große genug ist

            player.possible_lay_out.possible_layout_cards.push_back(cards.order[i]); // nur die Zahl wird gespeichert, nicht die Farbe
            player.possible_lay_out.amount_lay_out.push_back(player.amount[i]);
            player.possible_lay_out.sum_cardsblocks.push_back(player.amount_value[i]);
            continue;
        }
        // wenn Zahlkarte zwei Stück und mindestens ein Joker
        else if (i != 12 && i != 13) { // nicht für Joker
            if (    player.amount[i] >= min_amount_card &&	// nur wenn gleich/mehr als zwei Karten hat
                    cards.order[i] != "3" &&                // nur wenn keine drei ist
                    player.amount[i] + player.amount[12] + player.amount[13] >= 3 &&	// wenn mindest ein Joker hat
                    player.amount_value[i] + player.amount_value[12] + player.amount_value[13] >= min_value){ // nur wenn Summer der Karten+Joker groß genug ist
                player.possible_lay_out.possible_layout_cards.push_back(cards.order[i] + " mit Joker"); // nur die Zahl wird gespeichert, nicht die Farbe
                player.possible_lay_out.sum_cardsblocks.push_back(player.amount[i]);
                player.possible_lay_out.amount_lay_out.push_back(player.amount_value[i]);
                continue;
            }
        }
    }
//=> Alle Karten die der Spieler auslegen kann wurden gespeicher ine player.possible_lay_out

// Karten für Anzeige aktuellisieren und Text was auslegbar auch
    int eingabe_auslegen;
    if (player.possible_lay_out.possible_layout_cards.size()!=0) { // wenn man was auslegen kann
        // Aussage was auslegbar ist
        QString lay_out_print;
        backend -> setInfoText("\nWillst du was auslegen? Du kannst diese Karten auslegen:\n"); // Allgemeine Aussage zum auslegen
        for (int i = 0; i < player.possible_lay_out.possible_layout_cards.size(); i++) {
            lay_out_print += "Kartenblock "
                             + QString::number(i + 1) + ": "
                             + player.possible_lay_out.possible_layout_cards[i]
                             + " mit Wert: " + QString::number(player.possible_lay_out.sum_cardsblocks[i])
                             + " und amount: " + QString::number(player.possible_lay_out.amount_lay_out[i])
                             + " \n ";
        }
        backend -> setinfopanelText(lay_out_print); // alles was der Spieler auslegen kann
    }
    else {
        backend->setinfopanelText("Keine Karten zum Auslegen!"); // wenn nichts auslegbar ist
    }
    //=> setzte infopanelText auf den gerade erstellten Text
}


bool ROUND::execute_lay_out_cards(PLAYER& player, CARDS& cards, Backend* backend, int index, int amount_small_joker, int amount_big_joker){
// Erstellen Dictionary über alle Infos über den Kartenblock der ausgelegt wird
    lay_out_colour lay_out_results;
    lay_out_results.number = player.possible_lay_out.possible_layout_cards[index]; // Zahl vom Kartenblock der ausgelegt wird
    lay_out_results.amount = player.possible_lay_out.amount_lay_out[index]; // Anzahl der Karten, ohne Joker
    lay_out_results.amount_small_joker = amount_small_joker;                // Anzahl der kleinen Joker die ausgelegt werden
    lay_out_results.amount_big_joker = amount_big_joker;                    // Anzahl der großen Joker die ausgelegt werden

    qDebug()<<"Es wird Karte " << lay_out_results.number << " mit " << amount_small_joker << "kleinen und "
             << amount_big_joker << " großen Joker ausgelegt";

    // In player.street_pile speichern, wenn lay_out ein eine Straße ist
    if (lay_out_results.amount + lay_out_results.amount_small_joker + lay_out_results.amount_big_joker >= 7 // braucht mindestens 7 Karten für die Straße
        && lay_out_results.amount >= 4) {   // braucht mindestens vier Erkennungskarten
        qDebug() << "Du hast eine Straße lay_out. Sehr gut!";
        player.street_pile.push_back(lay_out_results);
    }
    // In player.lay_out speichern, wenn lay_out normal auslegt wird
    else{
        // Hinzufügen normaler Karten zum player.lay_out mit kompletten Namen (Zahl_of_Farbe)
        std::vector<QString> path_cards;
        for (int i = 0; i < player.cards.size(); i++) {
            QString number = player.cards[i].section('_', 0, 0);
            qDebug() << "Zahl die überprüft wird: " << number;
            // normale Karten
            if (lay_out_results.number == number && number != "2" && number != "joker") {
                path_cards.push_back(player.cards[i]);
                player.cards.erase(player.cards.begin() + i);
                i--;
            }

            // kleine Joker
            else if (number == "2" && amount_small_joker > 0) {
                path_cards.push_back(player.cards[i]);
                player.cards.erase(player.cards.begin() + i);
                amount_small_joker--;
                i--;
            }

            // große Joker
            else if (number == "joker" && amount_big_joker > 0) {
                path_cards.push_back(player.cards[i]);
                player.cards.erase(player.cards.begin() + i);
                amount_big_joker--;
                i--;
            }
        }

        // hinzufügen des Pfades für das Bild zum player.lay_out
        lay_out_results.path_cards = path_cards;
        player.lay_out.push_back(lay_out_results);
    }

    // Karten neusortieren, nicht weil unordentlich, sondern weil player.amount, player.amount_value nicht mehr stimmt
    sortieren(player, cards);

    // Will man mehr Kartenblöcke auslegen?
    return false;
}

bool ROUND::ende_spielzug(PLAYER& player, CARDS& cards, bool spiel_fertig) {
    std::cout << "Welche Karte willst du ablegen?" << std::endl;
    std::string karte_ablegen;
    std::getline(std::cin, karte_ablegen);                 // dann Eingabe einlesen

    auto it = std::find(player.cards.begin(), player.cards.end(), karte_ablegen);
    if (it != player.cards.end() && player.cards.size() > 1) {
        ablagestapel.insert(ablagestapel.begin(), karte_ablegen);
        player.cards.erase(it);
        std::cout << "Du hast " << karte_ablegen << " abgelegt.\n";
        std::cout << player.cards.size() << std::endl;
        if (player.cards.size() == 0) {
            std::cout << "Herzlichen Glückwunsch! Sie haben das Spiel gewonnen" << std::endl;
            spiel_fertig = true;
            return spiel_fertig;
        }
        else { return spiel_fertig; }
    }
    else if (it != player.cards.end() && player.cards.size() == 1) {
        std::cout << "Du hast nicht genug Karten.\n";
        return spiel_fertig;
    }
    else {
        std::cout << "Diese Karte hast du nicht!\n";
        ende_spielzug(player, ablagestapel, spiel_fertig);
    }
    return false;
}

void ROUND::count_points() {
    std::cout << "Hier werden die Punkte gezaehlt" << std::endl;
}

// Karten definieren und mischen und austeilen

// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //
// alles was den Verlauf des Spiels definiert: Start, Mittel, Ende
*/


