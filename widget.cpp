#include "widget.h"

#include <QPixmap>
#include <QDebug>
#include <QHBoxLayout>
#include <QPropertyAnimation>

// -------------------------------------------------------------------------------------------------------------------------------
// Definition für Klasse widget_info_panel
// Konstruktur
WHO_TURN_WIDGET::WHO_TURN_WIDGET(BACKEND* backend, QWidget *parent)
    : QWidget(parent), backend(backend)
{
    who_turn_layout = new QVBoxLayout(this); // mainLayout erstellen, gehört zum QWidget parent

    // Info Text
    who_turn_text = new QLabel(backend->who_turn_text()); // neues Label mit dem aktuellsten infoText() aus dem backend
    who_turn_layout->addWidget(who_turn_text);                 // Label zum mainLayout hinzufügen

    connect(backend, &BACKEND::who_turn_textChanged, // wenn im backend infoText() geändert wird, wird infoText aktuellisiert und
            who_turn_text, &QLabel::setText);        // aktuelles wird angezeigt

    // Panel
    panelLayout = new QVBoxLayout;  // neues Layout erschaffen
    who_turn_layout->addLayout(panelLayout); // Layout zum mainLayout hinzufügen

    connect(backend, &BACKEND::lay_out_textChanged, // wenn infopanelText ändert wird rebuild_panel ausgeführt
            this, &WHO_TURN_WIDGET::LAY_OUT_WIDGET);
}


// Funktionen
void WHO_TURN_WIDGET::LAY_OUT_WIDGET(const QString &text)
{
    //// text ist Kopie und nicht veränderbar
    qDebug() << text;
    QStringList lines = text.split("\n", Qt::SkipEmptyParts); // splitte den text wenn ein \n im text auftaucht und ignoriere leere Zeilen
    //=> lines ist nun Liste in der Art [Kartenblock 1:..., Kartenblock 2:..., ..., Kartenblock index]

    // Alten auslege Text und Buttons/Spinrad löschen
    QLayoutItem *child;
    while ((child = panelLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child; //
    }

    // Infotext für die beiden Joker-Spinboxen
    QLabel *label_small = new QLabel("Kleine Joker");
    QLabel *label_big = new QLabel("Große Joker");
    panelLayout -> addWidget(label_small);
    panelLayout -> addWidget(label_big);

    // Zeilen erschaffen mit line-Text, Button, 2*Spinboxen
    int index = 0; // Index vom Kartenblock
    for (const QString &line : lines) { // Schleife über alle [Kartenblock 1, ...]
        QWidget *rowWidget = new QWidget;               // neues Widget hinzufügen (einfach zu löschen)
        QHBoxLayout *row = new QHBoxLayout(rowWidget);  // Inhalt von der rowWidget

        // Elemente für derzeitige Zeile
        QLabel *label = new QLabel(line);                   // Text
        QPushButton *btn = new QPushButton("Bestätigen");   // Button
        QSpinBox *smallJoker = new QSpinBox;                // 1. Spin Box
        QSpinBox *bigJoker   = new QSpinBox;                // 2. Spin Box
        QLabel *errorLabel = new QLabel;                    // Error Text


        // Zeile mit Inhalt füllen
        row->addWidget(label);          // label (Kartenblock 1:...)
        if (line != "Keine Karten zum Auslegen!" && line != " ") { // Wenn Karten zum Auslegen vorhanden
            row->addWidget(btn);        // Button
            row->addWidget(smallJoker); // 1.Spinbox
            row->addWidget(bigJoker);   // 2.Spinbox
            row->addWidget(errorLabel); // Error Text
        }

        panelLayout->addWidget(rowWidget); // Inhalt zum panelLayout hinzufügen

        //=> derzeitige Zeile wird im panelLayout angezeigt

        // ObjectNames setzen
        smallJoker->setObjectName("smallJoker");
        bigJoker->setObjectName("bigJoker");
        errorLabel->setObjectName("errorLabel");
        // => Damit kannst du später mit: rowWidget->findChild<QSpinBox*>("smallJoker"); diese Widgets wiederfinden

        errorLabel->setStyleSheet("color:red;"); // Text Farbe von setzen

        int currentIndex = index;

        // Button Logik
        connect(btn, &QPushButton::clicked, this, [this, rowWidget, currentIndex]() {
            QSpinBox* smallJoker = rowWidget->findChild<QSpinBox*>("smallJoker");
            QSpinBox* bigJoker   = rowWidget->findChild<QSpinBox*>("bigJoker");
            QLabel* errorLabel   = rowWidget->findChild<QLabel*>("errorLabel");

            if (!smallJoker || !bigJoker || !errorLabel) return; // wenn er nichts findet

            int smallOut = smallJoker->value();
            int bigOut   = bigJoker->value();

            int ground = this->backend->player1.possible_lay_out.sum_cardsblocks[currentIndex];
            int min    = this->backend->player1.min_value;

            if (smallOut * 20 + bigOut * 50 + ground < min) {
                errorLabel->setText("Du brauchst mehr Joker!");
                return;
            }

            errorLabel->setStyleSheet("color:green;");
            errorLabel->setText("OK");

            // Auslegen durchführen
            //this->backend->round_procedure2(currentIndex, smallOut, bigOut);
            //this->backend->player1.be_outside = 1;
//
            //// Panel neu aufbauen, damit neue Optionen angezeigt werden
            //this->backend->round.possible_lay_out_cards(this->backend->player1, this->backend->cards, this->backend);
        });

        index++;
    }
}


// -------------------------------------------------------------------------------------------------------------------------------
// Definition für Klasse WIDGET_PILE_DRAW_PILE
// Konstruktor
WIDGET_PILE_DRAW_PILE::WIDGET_PILE_DRAW_PILE(BACKEND* backend, QWidget *parent)
    : QWidget(parent), backend(backend)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    // Draw Pile Karte zum Layout hinzufügen (Stapel)
    drawPile = new QLabel;

    QPixmap drawPixmap(":/cards_picture/cards_picture/kindpng_1537437.png"); // füge Bild hinzu
    if (drawPixmap.isNull()) {
        qDebug() << "Konnte Draw Pile Bild nicht laden";
        drawPixmap = QPixmap(80,120);
        drawPixmap.fill(Qt::red);
    }

    drawPile->setPixmap(drawPixmap.scaled(80,120));
    layout->addWidget(drawPile);

    // Discard Pile Karte zum Layout hinzufügen (Ablagestapel)
    discardPile = new QLabel;
    layout->addWidget(discardPile);

        // Funktion zum Updaten des Discard Pile
    auto update_discard_pile = [=]() {
        QString path = ":/cards_picture/cards_picture/" + backend->first_draw_pile() + ".png";
        QPixmap pix(path);
       if (pix.isNull()) {
            qDebug() << "Konnte Discard Pile Bild nicht laden:" << path;
            pix = QPixmap(80,120);
            pix.fill(Qt::red);
        }
       discardPile->setPixmap(pix.scaled(80,120));
    };

    update_discard_pile();

    // führe update_discard_pile aus wenn draw_pile geändert wird
    connect(backend, &BACKEND::first_draw_pileChanged,
            this, update_discard_pile);
}

// -------------------------------------------------------------------------------------------------------------------------------
// Definition für Klasse WIDGET_PLAYER_CARDS
// Konstruktor
WIDGET_PLAYER_CARDS::WIDGET_PLAYER_CARDS(BACKEND* backend, int player_index, QWidget *parent)
    : QWidget(parent), backend(backend), player_index(player_index)
// setzt backend, player_index auf richtiges backend, player_index
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    cardsLayout = new QHBoxLayout;
    layOutLayout = new QHBoxLayout;


    mainLayout->addLayout(layOutLayout);
    mainLayout->addLayout(cardsLayout);

    // Karten in der Hand
    connect(backend, &BACKEND::p1_cards_listChanged,
            this, &WIDGET_PLAYER_CARDS::update_cards);

    connect(backend, &BACKEND::p2_cards_listChanged,
            this, &WIDGET_PLAYER_CARDS::update_cards);


    // Ausgelegte Karten (nur für Spieler 1 aktuell)
    if (player_index == 1) {
        connect(backend, &BACKEND::p1_layout_listChanged, this, [=]() {
            QLayoutItem *item;
            while ((item = layOutLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }

            for (const QString &card : backend->p1_layout_list()) {
                CLICKABLE_LABEL *cardLabel = new CLICKABLE_LABEL(card);
                QPixmap pix(":/cards_picture/cards_picture/" + card + ".png");
                if (pix.isNull()) {
                    pix = QPixmap(80,120);
                    pix.fill(Qt::red);
                }

                cardLabel->setPixmap(pix.scaled(80,120));
                layOutLayout->addWidget(cardLabel);
            }
        });
    }
}

void WIDGET_PLAYER_CARDS::update_cards(){
    // wird ausgeführt wenn die Karten des Spieler 1 geändert werden
    // einmal alles löschen
    QLayoutItem *item;
    while ((item = cardsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // lösche alle alten Zustände
    all_cards.clear();
    raised_state.clear();

    QStringList cards;          // eigene Karten setzen
    QStringList cards_other_pc; // vom anderem Spieler Karten setzen

    bool i_am_p1 = backend->is_server();

    if (player_index == 1) {
        // eigenes Feld
        cards = i_am_p1 ? backend->p1_cards_list()
                        : backend->p2_cards_list();
    }
    else {
        // Gegner
        int count = i_am_p1 ? backend->p2_cards_list().size()
                            : backend->p1_cards_list().size();

        for (int i = 0; i < count; i++) {
            cards << "kindpng_1537437"; // Kartenrückseite
        }
    }

    for (const QString &card : cards) {//gehe alle Elemente von cards durch
        CLICKABLE_LABEL *card_clickable_label = new CLICKABLE_LABEL(card, this); // macht Karte anklickbar

        all_cards.append(card_clickable_label);     //// schauen was das ist
        raised_state[card_clickable_label] = false; // setzt Angehoben auf false für aktuelle Karte

        // ausführen von raise_cards wenn Karte angeklickt wird
        connect(card_clickable_label, &CLICKABLE_LABEL::clicked,
                this, &WIDGET_PLAYER_CARDS::raise_cards);

        // laden des Bildes
        QPixmap pix;
        pix.load(":/cards_picture/cards_picture/" + card + ".png");

        card_clickable_label->setPixmap(pix.scaled(80,120));    // Karten anzeigen
        cardsLayout->addWidget(card_clickable_label);           // Karten zum Layout hinzufügne
    }
}


void WIDGET_PLAYER_CARDS::raise_cards(CLICKABLE_LABEL* clicked_label)
{
    qDebug()<< "ich komme";
    if (player_index != 1) return; // nur eigene Karten
    qDebug() << raised_state[clicked_label];
    if(raised_state[clicked_label]==true && clicked_label->how_often == 1){
        qDebug() << "Du willst diese Karte auslegen, das ist super aber es fehlt noch die Funktion dafür. Dafür muss aber überprüft werden ob die Karten anlegbar ist";
        return;
    }
    // alle Karten zurücksetzen
    for (CLICKABLE_LABEL* l : all_cards) {
        if (raised_state[l]) {
            l->setContentsMargins(0, 0, 0, 0); // zurücksetzen
            raised_state[l] = false;
        }
    }
    qDebug() << clicked_label->how_often;
    qDebug() << clicked_label->card_name;

    // geklickte Karte
    bool raised = raised_state[clicked_label]; // nehme den derzeitigen Status ob oben oder unten ist
    qDebug() << clicked_label;


    if (raised) {
        // runter
        clicked_label->setContentsMargins(0, 0, 0, 0);
        raised_state[clicked_label] = false;
        clicked_label->how_often = 0;
    } else {
        // hoch
        clicked_label->setContentsMargins(0, -30, 0, 0);
        raised_state[clicked_label] = true;
        clicked_label->how_often = 1;
    }
}

WIDGET_TAKE_CARD_PILE::WIDGET_TAKE_CARD_PILE(BACKEND* backend, QWidget *parent)
    : QWidget(parent), backend(backend)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    // Button ob Karte nehmen oder Haufen nehmen hinzufügen
    QPushButton *take_card_btn = new QPushButton("Take Card");
    QPushButton *take_pile_btn = new QPushButton("Take Pile");

    take_card_btn->setVisible(true);
    take_pile_btn->setVisible(false);


    connect(backend, &BACKEND::take_card_pileChanged,
            this, [=]() {
                take_card_btn->show();
                take_pile_btn->show();
                take_pile_btn->setEnabled(backend->can_take_pile);
    });
    connect(take_card_btn, &QPushButton::clicked,
            [=](){
                qDebug()<< "Du willst eine Karte nehmen";
    });

    layout->addWidget(take_card_btn);
    layout->addWidget(take_pile_btn);
}
