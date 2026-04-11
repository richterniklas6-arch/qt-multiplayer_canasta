#include "mainwindow.h"
#include "backend.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QWidget>
#include <QDebug>

MAIN_WINDOW::MAIN_WINDOW(QWidget *parent)
    : QMainWindow(parent)
{
    backend = new BACKEND(this);

    QWidget *central = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(central);

    // -----------------------------
    // UI ELEMENTE
    // -----------------------------
    QPushButton *host_btn = new QPushButton("Host");
    QPushButton *join_btn = new QPushButton("Join");

    QLineEdit *ip_edit = new QLineEdit("127.0.0.1");

    // HOST NAME
    QLineEdit *name_player1 = new QLineEdit;
    name_player1->setPlaceholderText("Name Host");

    QPushButton *name_p1_btn = new QPushButton("Set Host Name");

    // CLIENT NAME
    QLineEdit *name_player2 = new QLineEdit;
    name_player2->setPlaceholderText("Name Client");

    QPushButton *name_p2_btn = new QPushButton("Set Client Name");

    QTextEdit *chat = new QTextEdit;
    chat->setReadOnly(true);

    QLineEdit *input = new QLineEdit;
    QPushButton *send_btn = new QPushButton("Send");

    QPushButton *start_game_btn = new QPushButton("Start Game");

    // -----------------------------
    // INITIAL STATE
    // -----------------------------
    name_player1->setEnabled(false);
    name_player2->setEnabled(false);

    name_p1_btn->setEnabled(false);
    name_p2_btn->setEnabled(false);

    start_game_btn->setEnabled(false);

    // -----------------------------
    // LAYOUT
    // -----------------------------
    layout->addWidget(host_btn);
    layout->addWidget(name_player1);
    layout->addWidget(name_p1_btn);

    layout->addWidget(join_btn);
    layout->addWidget(name_player2);
    layout->addWidget(name_p2_btn);

    layout->addWidget(ip_edit);
    layout->addWidget(chat);
    layout->addWidget(input);
    layout->addWidget(send_btn);
    layout->addWidget(start_game_btn);

    setCentralWidget(central);

    // -----------------------------
    // HOST BUTTON
    // -----------------------------
    connect(host_btn, &QPushButton::clicked, [=]() {

        backend->set_server_mode(true);
        backend->host_game();

        name_player1->setEnabled(true);
        name_p1_btn->setEnabled(true);

        host_btn->setEnabled(false);
        join_btn->setEnabled(false);

        chat->append("[INFO] Host gestartet");
    });

    // -----------------------------
    // CLIENT BUTTON
    // -----------------------------
    connect(join_btn, &QPushButton::clicked, [=]() {

        backend->set_server_mode(false);
        backend->join_game(ip_edit->text());

        name_player2->setEnabled(true);
        name_p2_btn->setEnabled(true);

        host_btn->setEnabled(false);
        join_btn->setEnabled(false);

        chat->append("[INFO] Verbinde als Client...");
    });

    // -----------------------------
    // HOST NAME SETZEN
    // -----------------------------
    connect(name_p1_btn, &QPushButton::clicked, [=]() {

        QString name = name_player1->text().trimmed();

        if (name.isEmpty()) {
            chat->append("[ERROR] Host Name leer!");
            return;
        }

        backend->player1.name = name;

        chat->append("[INFO] Host Name gesetzt: " + name);
        qDebug() << "Player1:" << name;
    });

    // -----------------------------
    // CLIENT NAME SETZEN + SENDEN
    // -----------------------------
    connect(name_p2_btn, &QPushButton::clicked, [=]() {

        QString name = name_player2->text().trimmed();

        if (name.isEmpty()) {
            chat->append("[ERROR] Client Name leer!");
            return;
        }

        backend->set_pending_name(name);

        // Name sofort an Server senden
        backend->send_message("NAME|" + name);

        chat->append("[INFO] Client Name gesendet: " + name);
        qDebug() << "Player2:" << name;
    });

    // -----------------------------
    // START GAME
    // -----------------------------
    connect(start_game_btn, &QPushButton::clicked, [=]() {
        qDebug() << "bin hier";
        backend->set_player_names(
            name_player1->text(),
            name_player2->text()
            );

        backend->start_game();
    });

    // -----------------------------
    // SEND CHAT
    // -----------------------------
    connect(send_btn, &QPushButton::clicked, [=]() {
        backend->send_message(input->text());
        chat->append("Ich: " + input->text());
        input->clear();
    });

    // -----------------------------
    // BACKEND → UI
    // -----------------------------
    connect(backend, &BACKEND::message_received, [=](QString msg){
        chat->append("Andere: " + msg);
    });

    connect(backend, &BACKEND::status_changed, [=](QString s){
        chat->append("[STATUS] " + s);

        // 🔥 HIER PASSIERT DIE MAGIE
        if (s.contains("Player2 gesetzt")) {
            start_game_btn->setEnabled(true);
        }
    });
}
