/**
* =============================================================================
*  MUSIK-BIBLIOTHEK – HEADER - MUSICMANAGER.HPP
* =============================================================================
*  Datei:        MusicLibrary.hpp
*  Projekt:      Einfache Musik-Bibliothek (CSV-basiert)
*  Inhalt:       Deklaration Datenstruktur MusicTrack und der Klasse
*                MusicLibrary mit Funktionen zum Laden/Speichern/Suchen
*
*  Datum:        2025-12-22
* 
*  Hinweis / Disclaimer:
*  Für die Gestaltung, Optimierung, Strukturierung sowie Unterstützung bei der
*  Fehlersuche wurde ChatGPT verwendet. Der Code wurde jedoch
*  eigenständig überprüft und angepasst, um den Projektanforderungen zu genügen.
* 
* =============================================================================
*/


#pragma once
// Verhindert dass HEader mehrfach eingebunden wird

#include <string>
#include <vector>
#include <optional>


//Musiktitel mit typischen Feldern.

struct MusicTrack {
    int id{ 0 };                // Track ID
    std::string title;          // Titel 
    std::string artist;         // Künstler
    std::string album;          // Album
    int year{ 0 };              // Erscheinungsjahr 
    std::string genre;          // Genre 
    int durationSec{ 0 };       // Länge in sek
};

// Enum mit klar abgegrenzten Werten ("scoped enum").

enum class Field { Any, Title, Artist, Album, Genre, Year };


// Bib die Tracks verwaltet mit folgenden funktionen
// - CSV laden/speichern
// - Tracks hinzufügen/ändern/löschen
// - Suchen und auflisten
class MusicLibrary {
public:
    // Lädt Daten aus CSV-Datei
    
    bool loadFromCsv(const std::string& path);

    // SpeiChert die Liste in CSV-Datei
    
    bool saveToCsv(const std::string& path) const;

    // neuen Track hinzufügen
   
    int  addTrack(const MusicTrack& t);

    // Track aktualisieren
    bool updateTrack(int id, const MusicTrack& t);

    // Track löschen
    bool deleteTrack(int id);

    // Sucht einen Track anhand ID.
   
    std::optional<MusicTrack> findById(int id) const;

    // Sucht Track nach eingegebenen Text
    std::vector<MusicTrack>   search(const std::string& query, Field by) const;

    // Liefert konst. Referenz auf alle Tracks.
   
    const std::vector<MusicTrack>& listAll() const { return tracks_; }

    // Löscht alle Tracks 
    void clear();

    // Hilfsfunktion, bereinigt einen String (z.b. Leerzeichen trimmen, problematische Zeichen entfernen)
    
    static std::string sanitize(const std::string& s);

    // Hilfsfunktion, Wandelt einen Track in eine CSV-Zeile um, nötig zum speichern
   
    static std::string toCsvRow(const MusicTrack& t);

    // Hilfsfunktion, liest eine CSV-Zeile und füllt daraus einen MusicTrack
    
    bool fromCsvRow(const std::string& row, MusicTrack& out);

private:
    // Interner Speicher: für Liste aller Tracks
    std::vector<MusicTrack> tracks_;

    // Nächste freie ID, benötigt für hinzufügen neuer Tracks
    int nextId_{ 1 };

    // Stellt sicher, dass nextId_ immer größer als alle vorhandenen IDs ist
 
    void refreshNextId_();
};

