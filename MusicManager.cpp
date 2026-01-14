
/**
* =============================================================================
*  MUSIK-BIBLIOTHEK – IMPLEMENTIERUNG - MUSICMANAGER.CPP
* =============================================================================
*  Datei:        MusicLibrary.cpp
*  Projekt:      Einfache Musik-Bibliothek 
*  Inhalt:       Implementierung Klasse MusicLibrary. Enthält Logik zum
*                Laden und Speichern im CSV-Format, Suche/Bearbeitung.
*
*  Datum:        2025-12-22
*
*  CSV-Hinweise:
*   - Einfaches Komma-getrenntes Format.
*   - Bei Bedarf können Anführungszeichen/escaping ergänzt werden.
*
*  Abhängigkeiten:
*   - <fstream>, <sstream>, <algorithm>, <iomanip> (je nach Implementierung)
*
* 
* Hinweis / Disclaimer:
*  Für die Gestaltung, Optimierung, Strukturierung sowie Unterstützung bei der
*  Fehlersuche wurde ChatGPT verwendet. Der Code wurde jedoch
*  eigenständig überprüft und angepasst, um den Projektanforderungen zu genügen.
* =============================================================================
*/


#include "MusicManager.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <optional>


//------------------------------------- Hilfsfunktionen----------------------------------------------


// Entfernt Leerzeichen am Anfang und Ende
std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }

    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        end--;
    }

    return s.substr(start, end - start);
}



bool icontains(const std::string& text, const std::string& search) {
    // Kopien anlegen
    std::string t = text;
    std::string s = search;

    // text -> lowercase
    for (size_t i = 0; i < t.size(); ++i) {
        unsigned char ch = static_cast<unsigned char>(t[i]);
        t[i] = static_cast<char>(std::tolower(ch));
    }

    // search -> lowercase
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char ch = static_cast<unsigned char>(s[i]);
        s[i] = static_cast<char>(std::tolower(ch));
    }

    // Teilstring-Suche (case-insensitiv durch die Umwandlung)
    return t.find(s) != std::string::npos;
}



// CSV-Zeile in Felder aufteilen
std::vector<std::string> splitCsv(const std::string& line) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, ',')) {
        parts.push_back(trim(item));
    }

    return parts;
}


//--------------------------------- Methoden der MusicLibrary---------------------------------------------------

bool MusicLibrary::loadFromCsv(const std::string& path) {           //Track aus CSV Datei laden
    clear();
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string header;
    std::getline(file, header); // Kopfzeile ignorieren

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        MusicTrack track;
        if (fromCsvRow(line, track)) {
            tracks_.push_back(track);
        }
    }

    refreshNextId_();
    return true;
}

bool MusicLibrary::saveToCsv(const std::string& path) const {       //Track in CSV Datei speichern
    std::ofstream file(path);
    if (!file.is_open()) return false;

    file << "id,title,artist,album,year,genre,durationSec\n";
    for (const auto& t : tracks_) {
        file << toCsvRow(t) << "\n";
    }

    return true;
}
    
int MusicLibrary::addTrack(const MusicTrack& t) {                   //neuen Track hinzufügen
    MusicTrack copy = t;
    copy.id = nextId_++;
    copy.title = sanitize(copy.title);
    copy.artist = sanitize(copy.artist);
    copy.album = sanitize(copy.album);
    copy.genre = sanitize(copy.genre);

    tracks_.push_back(copy);
    return copy.id;
}

bool MusicLibrary::updateTrack(int id, const MusicTrack& t) {       //Track aktualisieren
    for (auto& track : tracks_) {
        if (track.id == id) {
            track.title = sanitize(t.title);
            track.artist = sanitize(t.artist);
            track.album = sanitize(t.album);
            track.year = t.year;
            track.genre = sanitize(t.genre);
            track.durationSec = t.durationSec;
            return true;
        }
    }
    return false;
}
    
bool MusicLibrary::deleteTrack(int id) {                            //Track löschen
    for (auto it = tracks_.begin(); it != tracks_.end(); ++it) {
        if (it->id == id) {
            tracks_.erase(it);
            return true;
        }
    }
    return false;
}

std::optional<MusicTrack> MusicLibrary::findById(int id) const {    //Track suchen nach ID
    for (const auto& track : tracks_) {
        if (track.id == id) return track;
    }
    return std::nullopt;
}




std::vector<MusicTrack> MusicLibrary::search(const std::string& query, Field by) const {    //Track suchen nach string
    std::vector<MusicTrack> results;

    for (const auto& t : tracks_) {
        bool match = false;

        switch (by) {
        case Field::Any:
            match = icontains(t.title, query) ||
                icontains(t.artist, query) ||
                icontains(t.album, query) ||
                icontains(t.genre, query) ||
                (std::to_string(t.year) == query);
            break;
        case Field::Title:  match = icontains(t.title, query); break;
        case Field::Artist: match = icontains(t.artist, query); break;
        case Field::Album:  match = icontains(t.album, query); break;
        case Field::Genre:  match = icontains(t.genre, query); break;
        case Field::Year:   match = (std::to_string(t.year) == query); break;
        }

        if (match) results.push_back(t);
    }

    return results;
}



void MusicLibrary::clear() {                                            //Bib leeren
    tracks_.clear();
    nextId_ = 1;
}

std::string MusicLibrary::sanitize(const std::string& s) {
    std::string result = trim(s);
    for (auto& ch : result) {
        if (ch == '\r' || ch == '\n') ch = ' ';
    }
    return result;
}

std::string MusicLibrary::toCsvRow(const MusicTrack& t) {                   //Wandelt Trach zu CSV Zeile um
    std::ostringstream oss;
    oss << t.id << "," << t.title << "," << t.artist << "," << t.album << ","
        << t.year << "," << t.genre << "," << t.durationSec;
    return oss.str();
}

bool MusicLibrary::fromCsvRow(const std::string& row, MusicTrack& out) {        //Wandelt Track von CSV Zeiele um für UI
    auto cols = splitCsv(row);
    if (cols.size() != 7) return false;

    try {
        out.id = std::stoi(cols[0]);
        out.title = cols[1];
        out.artist = cols[2];
        out.album = cols[3];
        out.year = std::stoi(cols[4]);
        out.genre = cols[5];
        out.durationSec = std::stoi(cols[6]);
        return true;
    }
    catch (...) {
        return false;
    }
}

void MusicLibrary::refreshNextId_() {
    int maxId = 0;
    for (const auto& t : tracks_) {
        if (t.id > maxId) maxId = t.id;
    }
    nextId_ = maxId + 1;
}
