#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMap>
#include <QList>
#include <QPropertyAnimation>

// Forward Declaration:
// Wir sagen dem Compiler nur, dass es die Klasse BACKEND gibt,
// ohne die komplette Definition zu brauchen.
// → spart Compile-Zeit und verhindert unnötige Includes
class BACKEND;

// Hauptfenster der Anwendung (UI)
class MAIN_WINDOW : public QMainWindow
{
    Q_OBJECT

public:
    // Konstruktor des MainWindows
    // parent = übergeordnetes Widget (meist nullptr am Anfang)
    // Qt kümmert sich damit automatisch um Speicherverwaltung
    MAIN_WINDOW(QWidget *parent = nullptr);

    // Funktionen des MainWindows
    // um zwischen Chat und Game durch eine Button hinund her zuklicken
    void game_reiter(QVBoxLayout *main_layout);
    // um das Spiel spielbar zu machen
    void start_game_logic();

private: // Die Members für diese Klasse
    // Pointer auf das Backend (Netzwerk-Logik)
    // → UI spricht mit Backend über diesen Pointer
    // → Backend macht Server/Client/Kommunikation
    BACKEND *backend;

    // Fenster für alle Aufgaben
    QStackedWidget *stack; // Hauptfenster da drauf wird alles gestacked
    QWidget *start_page;     // Fenster für das Starten des Spiels (Host/Client, Namen,...)
    QWidget *game_page;      // Fenster für das Spiel (Karten der Spieler, Haufen,...)
    QWidget *game_container;
    QVBoxLayout *game_layout;

    // Timer für Übergang Start Page -> Game Page
    QTimer *timer;
// Members für startPage
    // --- Widgets für Start Page ---
    QLabel *start_info_label;
    QLineEdit *player1_edit;
    QLineEdit *player2_edit;

// Member für game_page
    QStackedWidget *game_stack; // Hauptfenster für game_page
    QPushButton *chat_button;   // Button um Chat zu erreichen
    QPushButton *game_button;   // Button um Spiel zu erreichen
    QPushButton *start_new_game_button; // Button um Spiel abzubrechen und neues zu starten


    // Funktionen die Unterschiedliche mainLayouts haben
    void set_up_start_page();  // 1.: Bildschirm mit wer ist wer, Namen setzen, Spiel starten
    void set_up_game_page();   // 2.: Bildschirm mit den Karten und dem Spiel
};

#endif
