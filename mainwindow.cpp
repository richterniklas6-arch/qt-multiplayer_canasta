#include "mainwindow.h"
#include "backend.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QWidget>

MAIN_WINDOW::MAIN_WINDOW(QWidget *parent)
    : QMainWindow(parent)
{
    backend = new BACKEND(this);

    QWidget *central = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(central);

    // -----------------------------
    // UI
    // -----------------------------
    QPushButton *host_btn = new QPushButton("Host");
    QPushButton *join_btn = new QPushButton("Join");
    QPushButton *start_game_btn = new QPushButton("Start game");

    QLineEdit *ip_edit = new QLineEdit("127.0.0.1");

    QLineEdit *name_player1 = new QLineEdit;
    name_player1->setPlaceholderText("Name player Host");

    QLineEdit *name_player2 = new QLineEdit;
    name_player2->setPlaceholderText("Name player Client");

    QTextEdit *chat = new QTextEdit;
    chat->setReadOnly(true);

    QLineEdit *input = new QLineEdit;
    QPushButton *send_btn = new QPushButton("Send");

    // -----------------------------
    // INITIAL STATE (IMPORTANT FIX)
    // -----------------------------
    auto reset_ui = [&]() {
        host_btn->setEnabled(true);
        join_btn->setEnabled(false);
        start_game_btn->setEnabled(false);

        name_player1->setEnabled(false);
        name_player2->setEnabled(false);

        backend->set_server_mode(false);
    };

    reset_ui();

    // -----------------------------
    // HOST
    // -----------------------------
    connect(host_btn, &QPushButton::clicked, this, [=]() {

        reset_ui();

        backend->set_server_mode(true);
        backend->host_game();

        name_player1->setEnabled(true);

        host_btn->setEnabled(false);
        join_btn->setEnabled(true);
    });

    // -----------------------------
    // CLIENT NAME VALIDATION
    // -----------------------------
    connect(name_player2, &QLineEdit::textChanged, [=](const QString &text) {

        join_btn->setEnabled(!text.trimmed().isEmpty());
    });

    // -----------------------------
    // JOIN
    // -----------------------------
    connect(join_btn, &QPushButton::clicked, this, [=]() {

        QString name = name_player2->text().trimmed();

        if (name.isEmpty()) {
            chat->append("[ERROR] Client Name fehlt!");
            return;
        }

        backend->set_server_mode(false);
        backend->set_pending_name(name);

        backend->join_game(ip_edit->text());

        name_player2->setEnabled(false);

        host_btn->setEnabled(false);
        join_btn->setEnabled(false);
    });

    // -----------------------------
    // BACKEND STATUS
    // -----------------------------
    connect(backend, &BACKEND::status_changed, this, [=](QString s) {

        chat->append("[STATUS] " + s);

        if (s.contains("Client verbunden") ||
            s.contains("Player2 gesetzt")) {

            start_game_btn->setEnabled(true);
        }
    });

    // -----------------------------
    // START GAME
    // -----------------------------
    connect(start_game_btn, &QPushButton::clicked, this, [=]() {

        backend->set_player_names(
            name_player1->text(),
            name_player2->text()
            );

        backend->start_game();
    });

    // -----------------------------
    // SEND
    // -----------------------------
    connect(send_btn, &QPushButton::clicked, this, [=]() {
        backend->send_message(input->text());
        chat->append("Ich: " + input->text());
        input->clear();
    });

    // -----------------------------
    // CHAT
    // -----------------------------
    connect(backend, &BACKEND::message_received, this, [=](QString msg){
        chat->append("Andere: " + msg);
    });

    // -----------------------------
    // LAYOUT
    // -----------------------------
    layout->addWidget(host_btn);
    layout->addWidget(name_player1);
    layout->addWidget(join_btn);
    layout->addWidget(name_player2);
    layout->addWidget(ip_edit);
    layout->addWidget(chat);
    layout->addWidget(input);
    layout->addWidget(send_btn);
    layout->addWidget(start_game_btn);

    setCentralWidget(central);
}

//#include "mainwindow.h"
//#include "backend.h"
//
//#include <QVBoxLayout>
//#include <QPushButton>
//#include <QLineEdit>
//#include <QTextEdit>
//#include <QWidget>
//
//// Konstruktor des Hauptfensters
//MAIN_WINDOW::MAIN_WINDOW(QWidget *parent)
//    : QMainWindow(parent)
//{
//    // Backend erstellen (enthält Netzwerklogik)
//    backend = new BACKEND(this);
//
//    // Zentrales Widget (alles kommt da rein)
//    QWidget *central = new QWidget;
//
//    // Layout (ordnet alle UI-Elemente untereinander an)
//    QVBoxLayout *layout = new QVBoxLayout(central);
//
//
//    // -----------------------------
//    // UI ELEMENTE ERSTELLEN
//    // -----------------------------
//
//    // Button: Host starten (Server)
//    QPushButton *host_btn = new QPushButton("Host");
//
//    // Button: Verbindung zu Host herstellen (Client)
//    QPushButton *join_btn = new QPushButton("Join");
//
//    // Eingabefeld für IP-Adresse
//    QLineEdit *ip_edit = new QLineEdit("127.0.0.1"); // localhost als Standard
//
//    // Eingabefeld für Spieler Namen
//    QLineEdit *name_player1 = new QLineEdit;
//    name_player1->setPlaceholderText("Name player Host");
//    name_player1->setEnabled(false);
//
//    QLineEdit *name_player2 = new QLineEdit;
//    name_player2->setPlaceholderText("Name player Client");
//    name_player2->setEnabled(false);
//
//    // Chatfenster (zeigt Nachrichten an)
//    QTextEdit *chat = new QTextEdit;
//    chat->setReadOnly(true); // Benutzer kann hier nichts eingeben
//
//    // Eingabefeld für Nachrichten
//    QLineEdit *input = new QLineEdit;
//
//    // Button zum Senden der Nachricht
//    QPushButton *send_btn = new QPushButton("Send");
//
//    // Button zum Starten des Spiels
//    QPushButton *start_game_btn = new QPushButton("Start game");
//    connect(backend, &BACKEND::server_mode_changed, this, [=](bool is_server){ // holt sich bool ob man Server ist
//            start_game_btn->setEnabled(is_server); // Button kann nur gedrückt werden wenn man HOST ist
//            });
//
//    // -----------------------------
//    // UI ZUM LAYOUT HINZUFÜGEN
//    // -----------------------------
//    layout->addWidget(host_btn);
//    layout->addWidget(name_player1);
//    layout->addWidget(join_btn);
//    layout->addWidget(name_player2);
//    layout->addWidget(ip_edit);
//    layout->addWidget(chat);
//    layout->addWidget(input);
//    layout->addWidget(send_btn);
//    layout->addWidget(start_game_btn);
//
//    // Setzt das Layout als Hauptinhalt des Fensters
//    setCentralWidget(central);
//
//
//    // -----------------------------
//    // BUTTON LOGIK (UI → BACKEND)
//    // -----------------------------
//
//    // Wenn "Host" gedrückt wird → Server starten, Name eingebbar
//    connect(host_btn, &QPushButton::clicked, [=]() {
//        backend->set_server_mode(true);     // setzt m_is_server auf true (dieser PC ist Server/ HOST)
//        name_player1->setEnabled(true);     // ermöglicht Eingabe Name für HOST
//        backend->host_game();               // startet Server
//        join_btn->setEnabled(false);
//        });
//
//    // Wenn "Join" gedrückt wird → mit Server verbinden, Name eingebbar
//    connect(join_btn, &QPushButton::clicked, [=]() {
//        backend->set_server_mode(false);               // setzt auf false somit ist er CLIENT
//        name_player2->setEnabled(true);                // Name eingebbar
//        backend->set_pending_name(name_player2->text()); // setze den Namen von Spieler 2
//        backend->join_game(ip_edit->text());           // trete Spiel bei mit der eingebeben ID
//        host_btn->setEnabled(false);                   // schalte den HOST Button aus
//        });
//
//    // Wenn "Send" gedrückt wird → Nachricht senden
//    connect(send_btn, &QPushButton::clicked, [=]() {
//        backend->send_message(input->text());      // Nachricht an Backend schicken
//        chat->append("Ich: " + input->text());     // Lokal anzeigen
//        input->clear();                            // Eingabefeld leeren
//    });
//
//    // Wenn "Start Game" gedrückt wird -> starte das Spiel
//    connect(start_game_btn, &QPushButton::clicked, [=](){
//        backend->set_player_names(name_player1->text(), name_player2->text());
//        backend->start_game();
//        //timer->start(1000);
//    });
//
//
//
//    // -----------------------------
//    // BACKEND → UI (Empfang)
//    // -----------------------------
//
//    // Wenn eine Nachricht vom anderen Spieler kommt
//    connect(backend, &BACKEND::message_received, [=](QString msg){
//        chat->append("Andere: " + msg); // im Chat anzeigen
//    });
//
//    // Statusmeldungen anzeigen (z.B. verbunden, wartet, etc.)
//    connect(backend, &BACKEND::status_changed, [=](QString s){
//        chat->append("[STATUS] " + s);
//    });
//}
//
