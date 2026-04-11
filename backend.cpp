#include "backend.h"
#include <QDebug>
#include <QHostAddress>

// Konstruktor
BACKEND::BACKEND(QObject *parent) : QObject(parent) {}


// ----------------------------------------------------
// HOST STARTEN
// ----------------------------------------------------
void BACKEND::host_game()
{
    server = new QTcpServer(this);

    connect(server, &QTcpServer::newConnection,
            this, &BACKEND::on_new_connection);

    if (server->listen(QHostAddress::Any, 12345)) {
        emit status_changed("Host gestartet (warte auf Verbindung)");
    }
}

// -----------------------------
// Setzt ob man HOST oder CLIENT ist
// -----------------------------
void BACKEND::set_server_mode(bool server)
{
    m_is_server = server;
    emit server_mode_changed(server);
}

// -----------------------------
// Bool ausgeben ob man HOST ist oder nicht
// -----------------------------
bool BACKEND::is_server() const {
    return m_is_server;
}


// ----------------------------------------------------
// SERVER: neuer Client verbindet sich
// ----------------------------------------------------
void BACKEND::on_new_connection()
{
    socket = server->nextPendingConnection();

    connect(socket, &QTcpSocket::readyRead,
            this, &BACKEND::on_ready_read);

    connect(socket, &QTcpSocket::disconnected, this, [=]() {
        qDebug() << "Client disconnected";
    });

    // wichtig: NICHT sofort Namen senden → erst CONNECTED abwarten
    emit status_changed("Client verbunden!");
}


// ----------------------------------------------------
// CLIENT VERBINDET SICH
// ----------------------------------------------------
void BACKEND::join_game(QString ip)
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead,
            this, &BACKEND::on_ready_read);

    connect(socket, &QTcpSocket::connected, this, [=]() {
        emit status_changed("Verbunden mit Server");

        // Client sendet eigenen Namen direkt nach Verbindung
        if (!pending_name.isEmpty()) {
            send_message("NAME|" + pending_name);
        }
    });

    socket->connectToHost(ip, 12345);

    emit status_changed("Verbinde zu " + ip);
}


// ----------------------------------------------------
// EMPFANGENE DATEN
// ----------------------------------------------------
void BACKEND::on_ready_read()
{
    if (!socket) return;

    QString msg = QString::fromUtf8(socket->readAll());

    qDebug() << "RAW MSG:" << msg;

    // -------------------------
    // NAME EMPFANGEN
    // -------------------------
    if (msg.startsWith("NAME|")) {
        QString name = msg.section('|', 1);

        player2.name = name;

        qDebug() << "Player2 Name erhalten:" << name;

        emit status_changed("Player2 gesetzt: " + name);
        return;
    }

    // -------------------------
    // normale Nachricht
    // -------------------------
    emit message_received(msg);
}


// ----------------------------------------------------
// SENDEN
// ----------------------------------------------------
void BACKEND::send_message(QString msg)
{
    if (socket && socket->isOpen()) {
        socket->write(msg.toUtf8());
    }
}


// ----------------------------------------------------
// NAME speichern (Client)
// ----------------------------------------------------
void BACKEND::set_pending_name(QString name)
{
    pending_name = name;
}


// ----------------------------------------------------
// SPIELER NAMEN (lokal Host UI)
// ----------------------------------------------------
void BACKEND::set_player_names(QString name1, QString name2)
{
    player1.name = name1;
    player2.name = name2;

    player1.min_value = 50;
    player2.min_value = 50;

    qDebug() << "Player1:" << player1.name;
    qDebug() << "Player2:" << player2.name;
}

// ----------------------------------------------------
// START GAME
// ----------------------------------------------------
void BACKEND::start_game()
{
    qDebug() << "is_server:" << is_server();
    qDebug() << "player1:" << player1.name;
    qDebug() << "player2:" << player2.name;

    if (!is_server()) {
        qDebug() << "Nur Host darf starten!";
        return;
    }

    if (player1.name.isEmpty() || player2.name.isEmpty()) {
        qDebug() << "Noch nicht alle Namen vorhanden!";
        return;
    }

    qDebug() << "🔥 SPIEL STARTET JETZT!";
}


//#include "backend.h"
//#include <QDebug>
//#include <QHostAddress>
//
//// Konstruktor der BACKEND-Klasse
//// Wird aufgerufen, wenn das Backend-Objekt erstellt wird
//BACKEND::BACKEND(QObject *parent) : QObject(parent) {}
//
//
//// -----------------------------
//// HOST (SERVER) STARTEN
//// -----------------------------
//void BACKEND::host_game()
//{
//    // Erstelle einen TCP-Server (wartet auf eingehende Verbindungen)
//    server = new QTcpServer(this);
//
//    // Verbinde das Signal "neue Verbindung" mit unserer Funktion onNewConnection
//    connect(server, &QTcpServer::newConnection,
//            this, &BACKEND::on_new_connection);
//
//    // Starte den Server auf Port 12345 und akzeptiere Verbindungen von überall
//    if(server->listen(QHostAddress::Any, 12345)) {
//        // Signal an GUI: Status anzeigen
//        emit status_changed("Host gestartet (warte auf Verbindung)");
//    }
//}
//
//
//// -----------------------------
//// Setzt ob man HOST oder CLIENT ist
//// -----------------------------
//void BACKEND::set_server_mode(bool server)
//{
//    m_is_server = server;
//    emit server_mode_changed(server);
//}
//
//// -----------------------------
//// CLIENT VERBINDET SICH ZUM HOST
//// -----------------------------
//void BACKEND::join_game(QString ip)
//{
//    // Erstelle einen TCP-Socket (Verbindung zu einem Server)
//    socket = new QTcpSocket(this);
//
//    // Wenn Daten ankommen → onReadyRead ausführen
//    connect(socket, &QTcpSocket::readyRead,
//            this, &BACKEND::on_ready_read);
//
//    // Verbinde dich mit der angegebenen IP auf Port 12345
//    socket->connectToHost(ip, 12345);
//
//    // Status an GUI senden
//    emit status_changed("Verbinde zu " + ip);
//
//    //
//    connect(socket, &QTcpSocket::connected, this, [=]() {
//        if (!pending_name.isEmpty()) {
//            send_message("NAME|" + pending_name);
//        }
//    });
//}
//
//
//// -----------------------------
//// WIRD AUFGERUFEN, WENN EIN CLIENT CONNECTET
//// (nur beim Host)
//// -----------------------------
//void BACKEND::on_new_connection()
//{
//    // Hole die eingehende Verbindung (Client)
//    socket = server->nextPendingConnection();
//
//    // Wenn der Client etwas sendet → onReadyRead
//    connect(socket, &QTcpSocket::readyRead,
//            this, &BACKEND::on_ready_read);
//
//    send_message("CONNECTED");// sende das verbunden ist
//
//    if (!pending_name.isEmpty()) {
//        send_message("NAME|" + pending_name);
//    }
//
//    // Status an GUI senden
//    emit status_changed("Client verbunden!");
//}
//
//
//// -----------------------------
//// WIRD AUFGERUFEN, WENN DATEN EMPFANGEN WERDEN
//// (Host UND Client)
//// -----------------------------
//void BACKEND::on_ready_read()
//{
//    // Lese alle verfügbaren Daten aus dem Socket
//    QString msg = QString::fromUtf8(socket->readAll());
//
//    if (msg == "CONNECTED") {
//        if (!pending_name.isEmpty()) { // wenn zu schickender Spieler2 Name nicht leer ist
//            send_message("NAME|" + pending_name); // schicke diesen Namen an den Server
//        }
//    }
//
//    // Setzen des Namens von Spieler 2
//    if (msg.startsWith("NAME|")) { // wenn eine msg mit "NAME|" beginnt
//
//        QString name = msg.section('|', 1); // nehmen alles ab |
//
//        player2.name = name;                // setze den Namen von Spieler 2 darauf
//
//        qDebug() << "Client Name erhalten:" << name;
//
//        return;
//    }
//
//    // Signal an GUI oder Spiellogik weitergeben
//    emit message_received(msg);
//}
//
//
//// -----------------------------
//// NACHRICHT SENDEN
//// -----------------------------
//void BACKEND::send_message(QString msg)
//{
//    // Nur senden, wenn Verbindung existiert
//    if(socket) {
//        // Nachricht als UTF-8 senden
//        socket->write(msg.toUtf8());
//    }
//}
//
////
//// setzt den Namen von Spieler 2 der geschickt wird
////
//void BACKEND::set_pending_name(QString name)
//{
//    pending_name = name;
//}
//
//// -----------------------------
//// Setzt die Namen der Spieler und die Mindestpunktzahl
//// -----------------------------
//void BACKEND::set_player_names(QString name1, QString name2){
//    player1.name = name1;
//    qDebug() << name2;
//    player1.min_value = 50;	// setze Startmindestpunktzahl auf 50
//    player2.min_value = 50;
//}
//
//// -----------------------------
//// Starte das Spiel
//// -----------------------------
//void BACKEND::start_game()
//{
//    qDebug() << "is_server:" << is_server();
//    qDebug() << "player1:" << player1.name;
//    qDebug() << "player2:" << player2.name;
//
//    if (!is_server()) {
//        qDebug() << "Nur Host darf das Spiel starten!";
//        return;
//    }
//
//    if (player1.name.isEmpty() || player2.name.isEmpty()) {
//        qDebug() << "Noch nicht alle Namen da!";
//        return;
//    }
//
//    qDebug() << "Spiel startet";
//}
//
//
//
