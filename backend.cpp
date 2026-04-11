#include "backend.h"
#include <QDebug>
#include <QHostAddress>

// Konstruktor der BACKEND-Klasse
// Wird aufgerufen, wenn das Backend-Objekt erstellt wird
BACKEND::BACKEND(QObject *parent) : QObject(parent) {}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                Server einrichten
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// HOST (SERVER) starten
void BACKEND::host_game()
{
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

// WIRD AUFGERUFEN, WENN DATEN EMPFANGEN WERDEN (Host UND Client)
// hier kann alles rein, wenn Daten geschickt werden, wie Namens Eingabe, Spielzüge,...
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


// ---------------------------------------------------------------------------------------------------------------------------------------------------
//                                                             Allgemeine Funktionen
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Nachrichten zwischen PC versenden
void BACKEND::send_message(QString msg)//bekommt eine QString rein
{
    if (socket && socket->isOpen()) {   // nur wenn Verbindung existiert und verbunden ist
        socket->write(msg.toUtf8());    // sende die msg an den anderen PC
    }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//                                                             Startseite einrichten
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// setzt Namen von Spieler 2
void BACKEND::set_pending_name(QString name_player2) // geschickter Name vom anderen PC rein
{
    pending_name = name_player2;    // setze Variable auf den Namen der geschickt wird
}

// -----------------------------
// Starte das Spiel
// -----------------------------
void BACKEND::start_game()
{
    qDebug() << "Bin ich der Server:" << m_is_server;   // Ausgabe wer der Server ist
    qDebug() << "player1:" << player1.name;             // Ausgabe wie player1 heißt
    qDebug() << "player2:" << player2.name;             // Ausgabe wie player2 heißt

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



