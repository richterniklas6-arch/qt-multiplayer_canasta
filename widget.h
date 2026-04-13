#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include "backend.h"
class CLICKABLE_LABEL; // sagen das das gibt aber später definiert wird


class WHO_TURN_WIDGET : public QWidget
{
    Q_OBJECT

public:
    explicit WHO_TURN_WIDGET(BACKEND* backend, QWidget *parent = nullptr);

private:
    BACKEND* backend;

    QVBoxLayout *who_turn_layout;
    QVBoxLayout *panelLayout;
    QLabel *who_turn_text;

    void LAY_OUT_WIDGET(const QString &text);
};


class WIDGET_PILE_DRAW_PILE : public QWidget
{
    Q_OBJECT

public:
    explicit WIDGET_PILE_DRAW_PILE(BACKEND* backend, QWidget *parent = nullptr);

private:
    BACKEND* backend;
    QLabel *drawPile;
    QLabel *discardPile;
};


class WIDGET_PLAYER_CARDS : public QWidget
{
    Q_OBJECT

public:

    int player_index; // Spielindex (sollte der sein von dem die Karten sichtbar sind)

    WIDGET_PLAYER_CARDS(BACKEND* backend, int player_index, QWidget *parent = nullptr);

    void update_cards();
    void raise_cards(CLICKABLE_LABEL* clickedLabel);

private:
    QHBoxLayout *cardsLayout;
    QHBoxLayout *layOutLayout;

    QHBoxLayout *cards_player2;
    QHBoxLayout *lay_out_player2;


    // QLabel ist der Name der Karte in Form Number_of_Colour
    QList<CLICKABLE_LABEL*> all_cards; // für jede Karte wird Anzahl und Liste der Label gespeichert
    QMap<QLabel*, bool> raised_state; // für jede Karte wird Label und true/false (angehoben?) gespeichert

    BACKEND* backend;
};


// Unterklasse um die Karten klickbar zu machen
class CLICKABLE_LABEL : public QLabel {
    Q_OBJECT
public:
    explicit CLICKABLE_LABEL(const QString &card_name, QWidget *parent = nullptr)
        : QLabel(parent), card_name(card_name) {}
    int how_often = 0;

    QString card_name;

signals:
    void clicked(CLICKABLE_LABEL* self);

protected:
    void mousePressEvent(QMouseEvent *) override {
        emit clicked(this);
    }
};

#endif // WIDGET_H
