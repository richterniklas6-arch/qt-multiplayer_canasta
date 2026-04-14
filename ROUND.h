#ifndef ROUND_H
#define ROUND_H
#include "PLAYER.h"
#include "CARDS.h"

class BACKEND;

class ROUND
{
public:
    ROUND();

    bool round_finished = false; // Angabe ob Runde vorbei ist
    QString current_turn; // Name des Spielers, welcher in derzeitigen Runde dran ist
    QString player_turn;


    void deal_out_cards(PLAYER& p1, PLAYER& p2, CARDS& cards);
    void sortieren(PLAYER& player, CARDS& cards);
    bool auslegen_definition(PLAYER player, CARDS k);
    bool take_card_pile(PLAYER& player, CARDS& cards);
    void possible_lay_out_cards(PLAYER& player, CARDS& cards, BACKEND* backend);
    bool execute_lay_out_cards(PLAYER& player, CARDS& cards, BACKEND* backend, int index, int amount_small_joker, int amount_big_joker);
    bool ende_spielzug(PLAYER& player, CARDS& cards, bool spiel_fertig);
    void count_points();
    void punkte_zaehlen();
};

#endif // ROUND_H

