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
    // Stacked Widget für Seiten
    stack = new QStackedWidget;
    setCentralWidget(stack);

    set_up_start_page();   // Startseite
    set_up_game_page();    // Spielseite mit den Karten


    // Startseite zuerst
    stack->setCurrentWidget(start_page);

    // Timer für Übergang Startseite -> game_page
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [=]() {
        stack->setCurrentWidget(game_page); // Übergang zu game_page
    });
}

void MAIN_WINDOW::set_up_start_page()
{
    start_page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(start_page);

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
        backend->start_game();
        backend->send_message("START_GAME");
        timer->start(1000);
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
    connect(backend, &BACKEND::message_received, [=](QString msg){//msg wird vom Host gesendet
        // wenn das Spiel gestartet wird
        if (msg == "START_GAME") {
            stack->setCurrentWidget(game_page);  // Client wechselt ins Spiel
            return;
        }

        // normale Messages
        chat->append("Andere: " + msg);
    });

    connect(backend, &BACKEND::status_changed, [=](QString s){
        chat->append("[STATUS] " + s);
// was macht das hier?

        if (s.contains("Player2 gesetzt")) {
            start_game_btn->setEnabled(true);
        }
    });


    stack->addWidget(start_page); // hier wird das ganze Layout zum Bildschirm hinzugefügt
}

void MAIN_WINDOW::set_up_game_page()
{
    game_page = new QWidget;
    QVBoxLayout *main_layout = new QVBoxLayout(game_page);
    // Menüband
    game_reiter(main_layout); // fügt den "Reiter" zwischen Chat und Game hin und her zu wechseln

    /*
    // ---------------- Info Panel ----------------
    widget_info_panel *info_panel = new widget_info_panel(backend);

    // Infopanel zum main_layout hinzufügen
    main_layout->addWidget(info_panel);
    //=> Infokasten darüber was auslegbar ist


    // ---------------- Cards ----------------
    WIDGET_PLAYER_CARDS *player2_widget = new WIDGET_PLAYER_CARDS(backend, 2);       // ausgelegte und Hand Karten Spieler 2
    WIDGET_PILE_DRAW_PILE *pile_draw_pile_widget = new WIDGET_PILE_DRAW_PILE(backend);  // Draw Pile and Discard Pile
    WIDGET_PLAYER_CARDS *player1_widget = new WIDGET_PLAYER_CARDS(backend, 1);       // ausgelegte und Hand Karten Spieler 1

    // Karten hinzufügen zum main_layout (Hauptebene Anzeigen
    main_layout -> addWidget(player2_widget);
    main_layout->addWidget(pile_draw_pile_widget);
    main_layout->addWidget(player1_widget);
    //=> alle Karten die es gibt zum main_layout hinzugefügt

    stack->addWidget(game_page);
    */
}

void MAIN_WINDOW::game_reiter(QVBoxLayout *main_layout){

    // -----------------------------
    // TOP BUTTON BAR
    // -----------------------------
    QHBoxLayout *top_bar = new QHBoxLayout;

    chat_button = new QPushButton("Chat");
    game_button = new QPushButton("Game");
    start_new_game_button = new QPushButton("New game");

    top_bar->addWidget(game_button);
    top_bar->addWidget(chat_button);
    top_bar->addWidget(start_new_game_button);

    main_layout->addLayout(top_bar);

    // -----------------------------
    // STACK IM GAME
    // -----------------------------
    game_stack = new QStackedWidget;

    QWidget *game_view = new QWidget;
    QWidget *chat_view = new QWidget;

    game_stack->addWidget(game_view);
    game_stack->addWidget(chat_view);

    main_layout->addWidget(game_stack);

    stack->addWidget(game_page);

    // -----------------------------
    // UI bauen
    // ... für Chat
    QVBoxLayout *chat_layout = new QVBoxLayout(chat_view);

    QTextEdit *chat_box = new QTextEdit;
    chat_box->setReadOnly(true);

    QLineEdit *input = new QLineEdit;
    QPushButton *send = new QPushButton("Send");

    chat_layout->addWidget(chat_box);
    chat_layout->addWidget(input);
    chat_layout->addWidget(send);

    connect(send, &QPushButton::clicked, [=]() {
        backend->send_message(input->text());
        chat_box->append("Ich: " + input->text());
        input->clear();
    });

    connect(backend, &BACKEND::message_received, [=](QString msg){
        chat_box->append("Andere: " + msg);
    });

    // ... für Game
    QVBoxLayout *gameLayout = new QVBoxLayout(game_view);

    QLabel *placeholder = new QLabel("Game läuft hier (Karten kommen später)");
    gameLayout->addWidget(placeholder);

    // -----------------------------
    // Umschalten zwischen Game und Chat und Start new Game
    // -----------------------------
    connect(chat_button, &QPushButton::clicked, this, [=]() {
        game_stack->setCurrentIndex(1); // Chat
    });

    connect(game_button, &QPushButton::clicked, this, [=]() {
        game_stack->setCurrentIndex(0); // Game
    });

    connect(start_new_game_button, &QPushButton::clicked, this, [=](){
        qDebug()<< "Du willst das Spiel neu starten, geht noch nicht";
    });
}

