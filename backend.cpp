#include "backend.h"
#include <QDebug>
#include <QHostAddress>

// Konstruktor der BACKEND-Klasse
// Wird aufgerufen, wenn das Backend-Objekt erstellt wird
BACKEND::BACKEND(QObject *parent) : QObject(parent) {}


// -----------------------------
// HOST (SERVER) STARTEN
// -----------------------------
void BACKEND::host_game()
{
    // Erstelle einen TCP-Server (wartet auf eingehende Verbindungen)
    server = new QTcpServer(this);

    // Verbinde das Signal "neue Verbindung" mit unserer Funktion onNewConnection
    connect(server, &QTcpServer::newConnection,
            this, &BACKEND::on_new_connection);

    // Starte den Server auf Port 12345 und akzeptiere Verbindungen von überall
    if(server->listen(QHostAddress::Any, 12345)) {
        // Signal an GUI: Status anzeigen
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
// CLIENT VERBINDET SICH ZUM HOST
// -----------------------------
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


// -----------------------------
// WIRD AUFGERUFEN, WENN EIN CLIENT CONNECTET
// (nur beim Host)
// -----------------------------
void BACKEND::on_new_connection()
{
    socket = server->nextPendingConnection();

    connect(socket, &QTcpSocket::readyRead,
            this, &BACKEND::on_ready_read);

    emit status_changed("Client verbunden!");
}


// -----------------------------
// WIRD AUFGERUFEN, WENN DATEN EMPFANGEN WERDEN
// (Host UND Client)
// -----------------------------
void BACKEND::on_ready_read()
{
    QString msg = QString::fromUtf8(socket->readAll());

    qDebug() << "RAW:" << msg;

    // -------------------------
    // NAME EMPFANGEN
    // -------------------------
    if (msg.startsWith("NAME|")) {

        QString name = msg.section('|', 1);

        player2.name = name;

        qDebug() << "Player2 Name erhalten:" << name;

        emit status_changed("Player2 gesetzt");

        return;
    }

    // -------------------------
    // normale Nachricht
    // -------------------------
    emit message_received(msg);
}


// -----------------------------
// NACHRICHT SENDEN
// -----------------------------
void BACKEND::send_message(QString msg)
{
    if (socket && socket->isOpen()) {
        socket->write(msg.toUtf8());
    }
}

//
// setzt den Namen von Spieler 2 der geschickt wird
//
void BACKEND::set_pending_name(QString name)
{
    pending_name = name;
}

// -----------------------------
// Setzt die Namen der Spieler und die Mindestpunktzahl
// -----------------------------
void BACKEND::set_player_names(QString name1, QString name2)
{
    player1.min_value = 50;
    player2.min_value = 50;
}

// -----------------------------
// Starte das Spiel
// -----------------------------
void BACKEND::start_game()
{
    qDebug() << "is_server:" << is_server();
    qDebug() << "player1:" << player1.name;
    qDebug() << "player2:" << player2.name;

    if (!is_server()) {
        qDebug() << "Nur Host darf das Spiel starten!";
        return;
    }

    if (player1.name.isEmpty() || player2.name.isEmpty()) {
        qDebug() << "Noch nicht alle Namen da!";
        return;
    }

    qDebug() << "Spiel startet";
}

// -----------------------------
// Bool ausgeben ob man HOST ist oder nicht
// -----------------------------
bool BACKEND::is_server() const {
    return m_is_server;
}

