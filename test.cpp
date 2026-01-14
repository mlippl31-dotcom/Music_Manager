#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "MusicManager.hpp"


//-------------------------------------------------UNIT-TESTS-----------------------------------------------------------


//Folgende Methoden nicht testbar: printTrack, readLine, promptTrack(), fieldFromInt() , da diese in main sind 
//Folgende Methoden testbar: addTrack, loadFromCSV, search, updateTrack, deleteTrack, sanitize, toCscRow, fromCsvRow



static MusicTrack makeTrack(                    //Hilfsfunktion für die erstellung von einem Track
    const std::string& title,
    const std::string& artist,
    const std::string& album,
    int year,
    const std::string& genre,
    int duration)
{
    MusicTrack t;
    t.title = title;
    t.artist = artist;
    t.album = album;
    t.year = year;
    t.genre = genre;
    t.durationSec = duration;
    return t;
}




TEST_CASE("addTrack vergibt fortlaufende ID richtig", "(Test Methode addTrack)") {                                                      
    MusicLibrary lib;

    int id1 = lib.addTrack(makeTrack("Song A", "Artist A", "Album A", 2020, "Pop", 180));       //erstellen von zwei Tracks                 
    int id2 = lib.addTrack(makeTrack("Song B", "Artist B", "Album B", 2021, "Rock", 200));      

    REQUIRE(id1 == 1);                                                                          //IDs überprüfen
    REQUIRE(id2 == 2);
    REQUIRE(lib.listAll().size() == 2);                                                         //Überprüfen ob nur zwei Tracks in neuer Bib sind
}





TEST_CASE("findById findet Track", "(Test Methode findById()") {                                  
    MusicLibrary lib;
    int id = lib.addTrack(makeTrack("Test", "Tester", "Album", 2022, "Jazz", 150));             //neuen Track in Bib speichern              

    auto result = lib.findById(id);                                                             //erstellten Track suchen   

    REQUIRE(result.has_value());                                                                //track gefunden
    REQUIRE(result->title == "Test");                                                           //Überprüfen der Metadaten  
    REQUIRE(result->artist == "Tester");    
}





TEST_CASE("findById für Fall das ID nicht vergeben ist", "(Test Methode findById()") {
    MusicLibrary lib;

    auto result = lib.findById(999);                                                            //Suchen nach nicht vorhandener ID               
    REQUIRE_FALSE(result.has_value());                                                          //muss False zurückgeben
}               





TEST_CASE("Aktualisieren MataDaten von Track", "(Test Methode updateTrack()") {
    MusicLibrary lib;
    int id = lib.addTrack(makeTrack("Alt", "AltArtist", "AltAlbum", 2000, "Pop", 100));         //Track alte MetaDaten

    MusicTrack neu = makeTrack("Neu", "NeuArtist", "NeuAlbum", 2023, "Rock", 220);              //Track neue Metadaten  

    bool ok = lib.updateTrack(id, neu);                                                         //Aktualisieren Track   
    REQUIRE(ok);                                                                                //Aktualisierung erfolgreich

    auto updated = lib.findById(id);
    REQUIRE(updated->title == "Neu");                                                           //Überprüfung ob neue MetaDaten gespeichert wurden
    REQUIRE(updated->year == 2022);
}





TEST_CASE("Aktualisieren MetaDaten von Track mit falscher ID Eingabe", "´(Test Methode updateTrack()") {
    MusicLibrary lib;
    MusicTrack t = makeTrack("X", "Y", "Z", 2000, "Pop", 100);

    REQUIRE_FALSE(lib.updateTrack(42, t));                                                      //Id nicht vergeben
}





TEST_CASE("Tracklöschen", "Test Methode deleteTrack") {                                         
    MusicLibrary lib;
    int id = lib.addTrack(makeTrack("Delete Me", "A", "B", 2010, "Pop", 120));                  //Track erstellen und in neuer Bib speichern

    REQUIRE(lib.deleteTrack(id));                                                               //erstellten Track löschen
    REQUIRE(lib.listAll().empty());                                                             //Bib muss nun wieder leer sein
}





TEST_CASE("Track löschen mit nicht vergebener ID", "´Test Methode deleteTrack") {
    MusicLibrary lib;                                                   
    REQUIRE_FALSE(lib.deleteTrack(123));                                                        //ID nicht vergeben muss False zurückgeben
}       






TEST_CASE("Tracksuche mit Titelname", "Test Methode search") {
    MusicLibrary lib;
    lib.addTrack(makeTrack("Hello World", "Alice", "Album1", 2020, "Pop", 180));                //Erstellen 2 neuer Tracks
    lib.addTrack(makeTrack("Goodbye", "Bob", "Album2", 2021, "Rock", 200));

    auto res = lib.search("hello", Field::Title);                                               //Suche nach Teilstring

    REQUIRE(res.size() == 1);
    REQUIRE(res[0].artist == "Alice");                                                          //Track gefunden
}





TEST_CASE("search mit alles druchsuchen", "Test Methode search") {                         
    MusicLibrary lib;   
    lib.addTrack(makeTrack("Song1", "Queen", "Best Of", 1980, "Rock", 210));                    //Erstellen von neuen Track

    auto res = lib.search("queen", Field::Any);                                                 //suchen nach Künstler z.b
    REQUIRE(res.size() == 1);                                                                   //Track gefunden
}





TEST_CASE("Bib leeren", "Test Methode clear") {
    MusicLibrary lib;
    lib.addTrack(makeTrack("A", "B", "C", 2000, "Pop", 100));                                   //Erstellen von zwei Tracks
    lib.addTrack(makeTrack("D", "E", "F", 2001, "Rock", 200));

    lib.clear();                                                                                //Bib leeren
    REQUIRE(lib.listAll().empty());                                                             //Bib muss leer sein
}






TEST_CASE("saveToCsv und loadFromCsv funktionieren zusammen", "test Funktion saveToCSV und load from CSV") {
    const std::string path = "test_library.csv";

    {
        MusicLibrary lib;
        lib.addTrack(makeTrack("CSV Song", "CSV Artist", "CSV Album", 2022, "Pop", 180));       //neuen Track erstellen 
        REQUIRE(lib.saveToCsv(path));                                                           //Track in CSV speichern
    }

    {
        MusicLibrary lib;
        REQUIRE(lib.loadFromCsv(path));                                                         //Track aus CSV laden
        REQUIRE(lib.listAll().size() == 1);
        REQUIRE(lib.listAll()[0].title == "CSV Song");                                          //Erfolgreiches Laden 
    }       

    std::remove(path.c_str());
}




















