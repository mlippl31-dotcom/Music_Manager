
/**
* ============================================================================ =
* MUSIK - BIBLIOTHEK – MAIN
* ============================================================================ =
* Datei:        main.cpp
* Projekt : Einfache Musik - Bibliothek
* Inhaltet :
* -Laden / Speichern der Bibliothek(über CSV Datei)
* -Anzeigen, Hinzufügen, Bearbeiten, Löschen
* -Suchen nach Begriff, ID
* -neue Bib laden
*
* Datum : 2025 - 12 - 22
*
* Bedienung :
* -Zahlen 0..7 im UI eingegeben werden
* -Pfade können übergeben werdne
* -Beim Beenden Möglichkeit zu speichern
* 
*  Hinweis / Disclaimer:
*  Für die Gestaltung, Optimierung, Strukturierung sowie Unterstützung bei der
*  Fehlersuche wurde ChatGPT verwendet. Der Code wurde jedoch
*  eigenständig überprüft und angepasst, um den Projektanforderungen zu genügen.
* 
* ============================================================================ =
*/



#include "MusicManager.hpp"
#include <iostream>
#include <limits>
#include <fstream>

//-------------------Hilfsfunktionen---------------------------------

// Druckt Track in einer kompakten Textzeile im UI

static void printTrack(const MusicTrack& t) {
    std::cout << "ID " << t.id << " | "
        << t.title << " — " << t.artist
        << " [" << t.album << ", " << t.year << "] "
        << t.genre << " | " << t.durationSec << "s\n";
}

// Liest ganze Zeile aus UI als String ein

static std::string readLine(const char* prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

// Liest Ganzzahl aus UI ein mit einfacher Fehlertoleranz

template<typename Int>
static Int readInt(const char* prompt) {
    std::cout << prompt;
    Int value{};
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Bitte eine Zahl eingeben: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

// Fragt interaktiv nach MetaDaten eines MusicTracks

static MusicTrack promptTrack() {
    MusicTrack t;

    t.title = readLine("Titel: ");
    t.artist = readLine("Artist: ");
    t.album = readLine("Album: ");
    t.year = readInt<int>("Jahr: ");
    t.genre = readLine("Genre: ");
    t.durationSec = readInt<int>("Dauer (Sekunden): ");

    return t;
}

// Wandelt eine einfache Feld-Auswahl in das Enum field

static Field fieldFromInt(int f) {
    switch (f) {
    case 1: return Field::Title;
    case 2: return Field::Artist;
    case 3: return Field::Album;
    case 4: return Field::Genre;
    case 5: return Field::Year;
    default: return Field::Any;
    }
}


// -----------------------------Hauptprogramm---------------------------------------------------

int main(int argc, char** argv) {
    // Standardpfad angeben, nötig wenn bei Start bereits Bibliothek mit Tracks vorhanden sein soll
    std::string path = "C:\\Software Engineering Labor\\MusicManager\\MusicManager\\library.csv";

    // Wenn ein Pfad Kommandozeilenargument übergeben wurde, übergebenen Pfad nnutzen
    if (argc >= 2) {
        path = argv[1];
    }

    MusicLibrary lib;

    // Versuche initial zu laden (load CSV)
    if (lib.loadFromCsv(path)) {
        std::cout << "Bibliothek geladen aus: " << path << "\n";
    }
    else {
        std::cout << "Keine bestehende Bibliothek gefunden. Eine neue wird gefuehrt unter: " << path << "\n";
    }

    // Einfaches Hauptmenü im UI, Auswahl für Benutzere
    bool running = true;
    while (running) {
        std::cout << "\n=== Musik-Bibliothek ===\n"
            << "Aktueller Pfad: " << path << "\n"
            << "1) Alle Titel anzeigen\n"
            << "2) Titel hinzufuegen\n"
            << "3) Titel bearbeiten\n"
            << "4) Titel loeschen\n"
            << "5) Suchen\n"
            << "6) Speichern\n"
            << "7) Andere Bibliothek laden (Pfad eingeben)\n"
            << "0) Beenden\n"
            << "Auswahl: ";

        int choice;
        if (!(std::cin >> choice)) {
            // Wenn hier die Eingabe fehlschlägt, wird beendet
            break;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1: {
            // Alle Titel anzeigen
            const auto& list = lib.listAll();
            if (list.empty()) {
                std::cout << "Keine Titel vorhanden.\n";
            }
            else {
                for (const auto& t : list) {
                    printTrack(t);
                }
            }
            break;
        }

        case 2: {
            // Titel hinzufügen
            std::cout << "Neuen Titel eingeben:\n";
            MusicTrack t = promptTrack();
            int id = lib.addTrack(t);
            std::cout << "Hinzugefuegt mit ID: " << id << "\n";
            break;
        }

        case 3: {
            // Titel bearbeiten
            std::cout << "ID zum Bearbeiten: ";
            int id;
            if (!(std::cin >> id)) { std::cin.clear(); }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            auto cur = lib.findById(id);
            if (!cur) {
                std::cout << "Nicht gefunden.\n";
                break;
            }

            std::cout << "Aktuell: ";
            printTrack(*cur);

            std::cout << "Neue Metadaten eingeben:\n";
            MusicTrack t = promptTrack();
            if (lib.updateTrack(id, t)) {
                std::cout << "Aktualisiert.\n";
            }
            else {
                std::cout << "Fehler beim Aktualisieren.\n";
            }
            break;
        }

        case 4: {
            // Titel löschen
            std::cout << "ID zum Loeschen: ";
            int id;
            if (!(std::cin >> id)) { std::cin.clear(); }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (lib.deleteTrack(id)) {
                std::cout << "Geloescht.\n";
            }
            else {
                std::cout << "Nicht gefunden.\n";
            }
            break;
        }

        case 5: {
            // Suchen mit Feld-Auswahl (0..5)
            std::string q = readLine("Suchbegriff: ");

            std::cout << "Feld (0=Any,1=Title,2=Artist,3=Album,4=Genre,5=Year): ";
            int f;
            if (!(std::cin >> f)) { std::cin.clear(); f = 0; }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            Field by = fieldFromInt(f);
            auto res = lib.search(q, by);

            if (res.empty()) {
                std::cout << "Keine Treffer.\n";
            }
            else {
                for (const auto& t : res) {
                    printTrack(t);
                }
            }
            break;
        }

        case 6: {
            // Speichern
            if (lib.saveToCsv(path)) {
                std::cout << "Gespeichert unter: " << path << "\n";
            }
            else {
                std::cout << "Speichern fehlgeschlagen.\n";
            }
            break;
        }

        case 7: {
            // neue Bibliothek laden (Pfad eingeben)
            std::string newPath = readLine("Neuen Pfad eingeben (z.B. C:\\Software Engineering Labor\\MusicManager\\MusicManager\\library.csv:\n> ");

            MusicLibrary newLib;
            if (newLib.loadFromCsv(newPath)) {
                lib = std::move(newLib);
                path = newPath;
                std::cout << "Bibliothek geladen aus: " << path << "\n";
            }
            else {
                std::cout << "Datei konnte nicht geladen werden.\n"
                    << "Neue (leere) Bibliothek unter diesem Pfad beginnen? (j/n): ";

                char ans;
                if (!(std::cin >> ans)) ans = 'n';
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (ans == 'j' || ans == 'J') {
                    lib.clear();
                    path = newPath;
                    std::cout << "Leere Bibliothek gesetzt. Du kannst Titel hinzufuegen und speichern.\n";
                }
                else {
                    std::cout << "Pfadwechsel abgebrochen.\n";
                }
            }
            break;
        }

        case 0: {
            // Optional beim Beenden speichern
            lib.saveToCsv(path);
            running = false;
            break;
        }

        default:
            std::cout << "Ungueltige Auswahl.\n";
        }
    }

    return 0;
}
