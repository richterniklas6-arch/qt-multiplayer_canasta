#include "backend.h"
#include <QDebug>
#include <QHostAddress>

// Konstruktor der BACKEND-Klasse
// Wird aufgerufen, wenn das Backend-Objekt erstellt wird
BACKEND::BACKEND(QObject *parent) : QObject(parent) {
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                Server einrichten
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// HOST (SERVER) starten
void BACKEND::host_game(){
    // Erstelle einen TCP-Server (wartet auf eingehende Verbindungen)
    server = new QTcpServer(this);

    // Verbinde das Signal "neue Verbindung" mit unserer Funktion on_new_connection
    connect(server, &QTcpServer::newConnection,
            this, &BACKEND::on_new_connection);

    // Starte den Server auf Port 12345 und akzeptiere Verbindungen von überall
    if(server->listen(QHostAddress::Any, 12345)) {
        // Signal an GUI: Status anzeigen
        emit status_changed("Host gestartet (warte auf Verbindung)");
    }
}

// Setzt ob man HOST oder CLIENT ist
void BACKEND::set_server_mode(bool server)
{
    m_is_server = server;               // setze die member (m_is_sever) auf false (ist nicht Server)/true (ist Server)
    emit server_mode_changed(server);   // lösen dieses Signal aus, nehme den Bool mit
}

//sende was man ist (Host oder Client)
bool BACKEND::is_server() const {
    return m_is_server;
}

// CLIENT VERBINDET SICH ZUM HOST
void BACKEND::join_game(QString ip)
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead,
            this, &BACKEND::on_ready_read);

    connect(socket, &QTcpSocket::connected, this, [=]() {
        emit status_changed("Verbunden mit Server");
    });

    socket->connectToHost(ip, 12345);

    emit status_changed("Verbinde zu " + ip);
}

// WIRD AUFGERUFEN, WENN EIN CLIENT CONNECTET (nur beim Host)
// hier kann alles rein, was passieren soll wenn ein neuer Client sich mit dem Host verbindet
void BACKEND::on_new_connection()
{

    socket = server->nextPendingConnection();

    connect(socket, &QTcpSocket::readyRead,
            this, &BACKEND::on_ready_read);

    emit status_changed("Client verbunden!");
}

// Erschaffen der Nachricht die geschickt wird (immer komplette Spielinfo)
QString BACKEND::create_state()
{
    QString state = "STATE|";           // startet immer mit STATE|

    // Player 1
    state += player1.name + "|";        // Name Spieler 1 hinzufügen
    QStringList p1_cards;
    for (const QString &card : player1.cards) {
        p1_cards << card;   // muss gemacht werden damit .join funktioniert
    }
    state += p1_cards.join(",") + "|";  // Karten Spieler 1 hinzufügen

    // Player 2
    state += player2.name + "|";        // Name Spieler 2 hinzufügen
    QStringList p2_cards;
    for (const QString &card : player2.cards) {
        p2_cards << card;   // muss gemacht werden damit .join funktioniert
    }
    state += p2_cards.join(",") + "|";  // Karten Spieler 2 hinzufügen

    // Draw pile
    if (!cards.draw_pile.empty())// nur wenn draw_pile nicht leer ist
        state += cards.draw_pile.back() + "|";  // Nachziehstapel oberste Karte hinzufügen
    else
        state += "EMPTY|";              //

    // Turn
    state += round.current_turn;        // wer in Runde dran ist hinzufügen

    return state;                       // gebe komplett alle Infos zurück
}

// Sende aktuellen Status an anderen PC
void BACKEND::send_state()
{
    if (!socket) return;                          // wenn Verbindung nicht steht, brich ab
    QString current_state = create_state();       // nehme Aktuellen Status des Spiels (alle Infos)
    socket->write((current_state+"\n").toUtf8()); // sende den State an den anderen PC
}

// WIRD AUFGERUFEN, WENN DATEN EMPFANGEN WERDEN (Host UND Client)
// hier kann alles rein, wenn Daten geschickt werden, wie Namens Eingabe, Spielzüge,...
void BACKEND::on_ready_read(){   
    QString data = QString::fromUtf8(socket->readAll()); // lese alle Nachrichten ein
    QStringList messages = data.split("\n", Qt::SkipEmptyParts); // splitte die Nachrichten wenn ein \n kommt
    //=> messages = [START_GAME, STATE|..., NAME|..., normale Nachricht]

    for (const QString &msg : messages) {// geht jedes Element der Liste durch
        qDebug() << "MSG:" << msg;

        // ------------------------
        // Starte das Spiel
        // -------------------------
        if (msg == "START_GAME") {          // das Spiel startet
            emit message_received(msg); // kann in UI angezeigt werden
        }
        // ------------------------
        // STATE EMPFANGEN
        // -------------------------
        else if (msg.startsWith("STATE|")) {// alle Infos über das Spiel
            apply_state(msg);           // setzt alle Infos des aktuellen Spiels
        }
        // -------------------------
        // NAME EMPFANGEN
        // -------------------------
        else if (msg.startsWith("NAME|")) { // Infos über die Namen der Spieler, damit ist Spiel startbar
            qDebug() << "Alle Namen sind da, das Spiel kann gestartet werden";
            QString name = msg.section('|', 1);
            player2.name = name;
            emit status_changed("Player2 gesetzt");
        }
        // -------------------------
        // normale Nachricht
        // -------------------------
        else {                              // normale Nachricht
            emit message_received(msg); // kann in UI angezeigt werden
        }
    }
}

// State des gesamten Spiels anwenden bzw. setzen
void BACKEND::apply_state(const QString &msg)
{
    QStringList parts = msg.split("|"); // teile jede Info von State auf und speicher als Liste

    int amount_infos = 7;
    if (parts.size() < amount_infos){  // State muss mindestens 7 Infos mit sich führen
        qDebug() << "STATE ist kaputt!! ODER hat mehr Infos als bisher: " << parts;
        qDebug() << "Bisherige Anzahl an Infos: " << amount_infos;
        qDebug() << "Derzeitig übertrage Infos: " << parts.size();
        return;
    }

    // setze die Namen der Spieler
    player1.name = parts[1];
    player2.name = parts[3];

    // setzt die Handkarten der Spieler
    QStringList p1 = parts[2].split(",", Qt::SkipEmptyParts);
    QStringList p2 = parts[4].split(",", Qt::SkipEmptyParts);
    if (m_is_server) {  // nur wenn man Server ist
        setP1_cards_list(p1); // setzt man von allen die Karten
        setP2_cards_list(p2);
    } else {            // wenn man nicht der Server ist
        setP2_cards_list(p2); // setzt man nur eigene Karten (Spieler 2)

        QStringList hidden;
        for (int i = 0; i < p1.size(); i++)
            hidden << "kindpng_1537437";
        setP1_cards_list(hidden);   // man kennt die Anzahl der Karten von Spieler 1
        //=> Client sieht Gegnerkarten nicht
    }

    // oberste Karte des Draw pile
    setFirst_draw_pile(parts[5]);

    // wer ist dran sezen
    round.current_turn = parts[6];
    setWho_turn_text("Am Zug: " + round.current_turn);

    qDebug() << "STATE angewendet!";
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
//                                                             Allgemeine Funktionen
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Nachrichten zwischen PC versenden
void BACKEND::send_message(QString msg)//bekommt eine QString rein
{
    if (socket && socket->isOpen()) {   // nur wenn Verbindung existiert und verbunden ist
        socket->write((msg+"\n").toUtf8());    // sende die msg an den anderen PC mit einem Absatz
    }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//                                                         Q_PROPERTY Funktionen
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void BACKEND::setWho_turn_text(const QString& text) {
    if (m_who_turn_text == text)
        return;
    m_who_turn_text = text;
    emit who_turn_textChanged(m_who_turn_text);
}
void BACKEND::setLay_out_text(const QString& text){
    if (m_lay_out_text == text)
        return;
    m_lay_out_text = text;
    emit lay_out_textChanged(m_lay_out_text);
}
void BACKEND::setP1_cards_list(const QStringList& cards){
    if (m_player1_cards_list == cards)
        return;
    m_player1_cards_list = cards;
    emit p1_cards_listChanged();
}
void BACKEND::setP1_layout_list(const QStringList& cards){
    if (m_player1_lay_out_list == cards)
        return;
    m_player1_lay_out_list = cards;
    emit p1_layout_listChanged();
}
void BACKEND::setP2_cards_list(const QStringList& cards){
    if (m_player2_cards_list == cards)
        return;
    m_player2_cards_list = cards;
    emit p2_cards_listChanged();
}
void BACKEND::setP2_layout_list(const QStringList& cards){
    if (m_player2_lay_out_list == cards)
        return;
    m_player2_lay_out_list = cards;
    emit p2_layout_listChanged();
}
void BACKEND::setFirst_draw_pile(const QString& cards){
    if (m_first_card_draw_pile == cards)
        return;
    m_first_card_draw_pile = cards;
    emit first_draw_pileChanged();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//                                                             Startseite einrichten
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// setzt Namen von Spieler 2
void BACKEND::set_pending_name(QString name_player2) // geschickter Name vom anderen PC rein
{
    pending_name = name_player2;    // setze Variable auf den Namen der geschickt wird
}

// Bedingungnen an den Button Start Game
void BACKEND::start_game(){
    //
    qDebug() << "Du bist der Server:" << m_is_server;   // Ausgabe wer der Server ist
    qDebug() << "Name von player1:" << player1.name;             // Ausgabe wie player1 heißt
    qDebug() << "Name von player2:" << player2.name;             // Ausgabe wie player2 heißt

    if (!m_is_server) { // nur wenn man selbst Host ist kann Spiel starten
        qDebug() << "Nur Host darf das Spiel starten!";
        return;
    }

    if (player1.name.isEmpty() || player2.name.isEmpty()) { // nur wenn von beiden Spielern der Name da ist
        qDebug() << "Noch nicht alle Namen da!";
        return;
    }

    qDebug() << "Spiel startet";
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
//                                                             Spielseite einrichten
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Setzt wer Spiel beginnt
void BACKEND::decide_who_begins() {
    int number_player = 2; // Anzahl der Spieler
    int random_beginner = rand() % number_player; // wähle zufällig 1 oder 2

    if (random_beginner == 0) { // wenn Spieler 1 beginnt
        game.player_first_draw = player1.name;  // setze den Spieler der begonnen hat
        round.current_turn = player1.name;      // setze den Spieler der die Runde beginnt
    }
    else {  // wenn Spieler 2 beginnt
        game.player_first_draw = player2.name;  // setze den Spieler der begonnen hat
        round.current_turn = player2.name;      // setze den Spieler der die Runde beginnt
    }

    if (m_is_server){send_state();} // sendet den aktuellen Status des Spiels an beide PC vom HOST aus
}

// teilt die Karten an beide Spieler aus
void BACKEND::deal_out_cards(){
    round.deal_out_cards(player1, player2, cards); // teilt Karten aus

    // setze die Q_PROPERTY auf den derzeitigen Stand
    // tut die letzte (oberste) Karte vom Ablagestapel übergeben (für anzeige der Karten)
    if (!cards.draw_pile.empty()) { // der Stapel darf nicht leer sein
        setFirst_draw_pile(cards.draw_pile.back());
    }
    // Liste erstellen mit den Namen der Karten (für anzeigen der Karten)
    QStringList list_player1;
    for (const QString &card : player1.cards) {
        list_player1 << card;
    }
    setP1_cards_list(list_player1); // setzt die Liste von player1 auf die ausgelegte Karten
    QStringList list_player2;
    for (const QString &card : player2.cards) {
        list_player2 << card;
    }
    setP2_cards_list(list_player2); // setzt die Liste von player2 auf die ausgelegte Karten

    if (m_is_server){send_state();} // sendet den aktuellen Status des Spiels an alle PC vom HOST aus
    emit p1_cards_listChanged();    // sage das Player 1 Karten geändert wurden
    emit p2_cards_listChanged();    // sage das Player 2 karten geändert wurden
}

//
bool BACKEND::round_procedure(){
    bool round_finished = false;
    //std::vector<PLAYER*> whole_player;
//
    //// könnte hier mit for schleife mehrere spieler hinzufügen
    //whole_player.push_back(&player1);
    //whole_player.push_back(&player2);
//
    //// sagen, wer dran ist
    //setInfoText("Es beginnt: " + game.player_first_draw);
//
    //// Schleife über die Spiele
    //for(PLAYER* player: whole_player){
    //    round.player_turn = player -> name; // setzen welcher Spieler dran ist
    //    print_order(); // drucken aller Karten und Werte von ihnen
//
    //    // Haufen oder Karte nehmen
    //    round.take_card_pile(*player, cards);
//
    //    //aktuellisiere die Karten des Spielers
    //    QStringList list_player;
    //    for (const QString &card : player->cards) {
    //        list_player << card;
    //    }
//
    //    // aktuellisiere für den Bildschirm die Karten der beiden Spieler
    //    if(player->name==player1.name)setcardsplayer1(list_player);
    //    if(player->name==player2.name)setcardsplayer2(list_player);
    //    round.possible_lay_out_cards(*player, cards, this);
    //    //round_procedure1(player, cards);
    //    break;
}
