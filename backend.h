#ifndef BACKEND_H
#define BACKEND_H
#include "player.h"
//#include "ROUND.h"
#include "game.h"

#include <QObject>
#include <QTcpServer>   // Für Server (Host)
#include <QTcpSocket>   // Für Verbindung (Client + Kommunikation)

// Backend = kümmert sich nur um Netzwerk (keine UI!)
class BACKEND : public QObject
{
    Q_OBJECT

//----------------------------------------------------------------------------------------------------------------------------------
//                                                     Alles was Public ist
//----------------------------------------------------------------------------------------------------------------------------------

public:
    // Konstruktor
    explicit BACKEND(QObject *parent = nullptr);

    // -------------------------------
    // Erschaffung der Elemente von allen wichtigen Klassen
    // -------------------------------
    PLAYER player1;
    PLAYER player2;
    //ROUND round;
    //CARDS cards;
    GAME game;

    // -------------------------------
    // Variablen zum senden zwischen CLIENT und HOST
    // -------------------------------
    QString pending_name; // Name von Spieler 2 merken bis Verbingdung zum Server steht
    void set_pending_name(QString name); // setzt den zumerkenen Spieler 2 Namen

    // -------------------------------
    // Netzwerk-Funktionen
    // -------------------------------

    // Startet das Spiel als Host (Server)
    // → wartet darauf, dass sich ein Client verbindet
    void host_game();

    // Verbindet sich als Client zu einem Server
    // ip = IP-Adresse des Hosts (z.B. "127.0.0.1")
    void join_game(QString ip);

    // Sendet eine Nachricht an den verbundenen Partner
    // (egal ob Server oder Client)
    void send_message(QString msg);

    // Startet das Spiel: Namenswahl, Spielfeld, ...
    void start_game();

    // Setzt ob man HOST oder CLIENT ist
    void set_server_mode(bool server);

    // gibt aus ob man HOST ist oder nicht
    bool is_server() const;

    // durch Slot Interaktion werden die Namen der Spieler durch eine eingabe gesetzt
    void set_player_names(QString name1, QString name2);

signals:
    // -------------------------------
    // Kommunikation Richtung UI
    // -------------------------------

    // Wird ausgelöst, wenn eine Nachricht vom anderen Spieler kommt
    // → UI kann das anzeigen (z.B. Chat oder später Spielzug)
    void message_received(QString msg);

    // Wird ausgelöst, wenn sich der Verbindungsstatus ändert
    // → z.B. "Host gestartet", "Client verbunden"
    void status_changed(QString status);

    // Wird ausgelöst, wenn sich Status ob HOST oder Client gemacht wird
    //-> übergibt ob man HOST oder CLIENT ist
    void server_mode_changed(bool is_server);

public slots:
    // durch Slot Interaktion werden die Namen der Spieler durch eine eingabe gesetzt
    //void set_player_names(QString name1, QString name2);

//----------------------------------------------------------------------------------------------------------------------------------
//                                                     Alles was Private ist
//----------------------------------------------------------------------------------------------------------------------------------
private slots:
    // -------------------------------
    // Interne Netzwerk-Events
    // -------------------------------

    // Wird aufgerufen, wenn sich ein Client mit dem Server verbindet
    // → nur relevant im Host-Modus
    void on_new_connection();

    // Wird aufgerufen, wenn Daten über das Netzwerk ankommen
    // → liest die Nachricht und gibt sie weiter
    void on_ready_read();

private:
    // -------------------------------
    // Netzwerk-Objekte
    // -------------------------------

    // Server-Objekt (nur aktiv wenn wir Host sind)
    QTcpServer *server = nullptr;

    // Socket = Verbindung zwischen zwei Spielern
    // → wird sowohl vom Server als auch vom Client benutzt
    QTcpSocket *socket = nullptr;

    // Member von backend die aussgat ob man HOST (true) oder Client (false) ist
    bool m_is_server = false;
};

#endif
