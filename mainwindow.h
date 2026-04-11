#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    // Pointer auf das Backend (Netzwerk-Logik)
    // → UI spricht mit Backend über diesen Pointer
    // → Backend macht Server/Client/Kommunikation
    BACKEND *backend;
};

#endif
