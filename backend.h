#ifndef BACKEND_H
#define BACKEND_H
#include "player.h"
#include "ROUND.h"
#include "game.h"
#include "CARDS.h"

#include <QObject>
#include <QTcpServer>   // Für Server (Host)
#include <QTcpSocket>   // Für Verbindung (Client + Kommunikation)

struct GameState {
    QString p1_name;
    QStringList p1_cards;

    QString p2_name;
    QStringList p2_cards;

    QString draw_pile_top;

    QString current_turn;
};

// Backend = kümmert sich nur um Netzwerk (keine UI!)
class BACKEND : public QObject
{
    Q_OBJECT
// Strings und Listen die verändert werden im Spiel
    // Strings die veränderliche Text enthalten
    // welcher Spieler ist derzeit dran
    Q_PROPERTY(QString who_turn_text    READ who_turn_text  WRITE setWho_turn_text      NOTIFY who_turn_textChanged FINAL)
    // was kann der Spieler derzeit auslegen
    Q_PROPERTY(QString lay_out_text     READ lay_out_text   WRITE setLay_out_text       NOTIFY lay_out_textChanged FINAL)
    // Karte des Ablagestapels
    Q_PROPERTY(QString first_draw_pile  READ first_draw_pile WRITE setFirst_draw_pile   NOTIFY first_draw_pileChanged FINAL)

    // Listen die die Dateinamen für die Karten haben
    // Handkarten des Spielers 1
    Q_PROPERTY(QStringList p1_cards_list READ p1_cards_list WRITE setP1_cards_list      NOTIFY p1_cards_listChanged FINAL)
    // Handkarten des Spielers 2
    Q_PROPERTY(QStringList p2_cards_list READ p2_cards_list WRITE setP2_cards_list      NOTIFY p2_cards_listChanged FINAL)
    // ausgelegte Karten des Spielers 1
    Q_PROPERTY(QStringList p1_layout_list READ p1_layout_list WRITE setP1_layout_list   NOTIFY p1_layout_listChanged FINAL)
    // ausgelegte Karten des Spielers 2
    Q_PROPERTY(QStringList p2_layout_list READ p2_layout_list WRITE setP2_layout_list   NOTIFY p2_layout_listChanged FINAL)

//----------------------------------------------------------------------------------------------------------------------------------
//                                                     Alles was Public ist
//----------------------------------------------------------------------------------------------------------------------------------

public:
// Konstruktor
    explicit BACKEND(QObject *parent = nullptr);


// Definitionen der READ für die Q_PROPERTY
    // geben nur die Member zruück
    QString who_turn_text() const {return m_who_turn_text;}
    QString lay_out_text() const {return m_lay_out_text;}
    QString first_draw_pile() const {return m_first_card_draw_pile;}
    QStringList p1_cards_list() const {return m_player1_cards_list;}
    QStringList p1_layout_list() const{return m_player1_lay_out_list;}
    QStringList p2_cards_list() const {return m_player2_cards_list;}
    QStringList p2_layout_list() const{return m_player2_lay_out_list;}

// Definition der WRITE für die Q_PROPERTY
    // setzt eine Member_String auf text den man haben will, auf diesen Text kann main.qml zugreifen und drucken
    void setWho_turn_text(const QString &text);
    void setLay_out_text(const QString &text);
    void setFirst_draw_pile(const QString& cards);
    // setzt Member-Liste auf liste die man haben will, auf diese Liste kann main.qml zugreifen und drucken
    void setP1_cards_list(const QStringList& cards);
    void setP1_layout_list(const QStringList& cards);
    void setP2_cards_list(const QStringList& cards);
    void setP2_layout_list(const QStringList& cards);


// -------------------------------
// Erschaffung der Elemente von allen wichtigen Klassen
// -------------------------------
    PLAYER player1; // alles für Spieler 1
    PLAYER player2; // alles für Spieler 2
    ROUND round;    // alles für aktuelle Runde
    CARDS cards;    // alles für Karten (Stapel, ...)
    GAME game;      // alles für gesamtes Spiel
    GameState state;// alles für das senden zwischen Host und Client

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

    bool is_server() const;

    // Setzt ob man HOST oder CLIENT ist
    void set_server_mode(bool server);

    // Verbindet sich als Client zu einem Server
    // ip = IP-Adresse des Hosts (z.B. "127.0.0.1")
    void join_game(QString ip);

    QString create_state();
    void send_state();
    void apply_state(const QString& msg);

// -------------------------------
// Allgemeine-Funktionen
// -------------------------------
    // Sendet eine Nachricht an den verbundenen Partner
    // (egal ob Server oder Client)
    void send_message(QString msg);

// -------------------------------
// Spiel-Funktionen
// -------------------------------
    // Startet das Spiel: Namenswahl, Spielfeld, ...
    void start_game();

    // entscheide wer beginnt
    void decide_who_begins();

    // teilt Karten aus
    void deal_out_cards();

    // tut eine Runde durchführen (1.Teil)
    bool round_procedure();


signals:
    // -------------------------------
    // Kommunikation zwischen Spieler und Spiel (Q_PROPERTY)
    // -------------------------------
    void who_turn_textChanged(const QString &text);
    void lay_out_textChanged(const QString &text);
    void first_draw_pileChanged();
    void p1_cards_listChanged();
    void p1_layout_listChanged();
    void p2_cards_listChanged();
    void p2_layout_listChanged();

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

    QString m_who_turn_text;
    QString m_lay_out_text;
    QString m_first_card_draw_pile;
    QStringList m_player1_cards_list;
    QStringList m_player1_lay_out_list;
    QStringList m_player2_cards_list;
    QStringList m_player2_lay_out_list;

};

#endif
