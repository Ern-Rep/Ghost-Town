#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <conio.h>
#include ".\ynot\utf8except.cpp"
#include ".\ynot\utf8conv.cpp"
#include ".\ynot\terminal.cpp"
#include ".\ynot\str.cpp"
#include ".\ynot\Inputfield.cpp"
#include ".\rapidjson\document.h"

bool IsNewline( char a ) {  return ( a == '\n' ); }

std::vector<std::string> hist;
size_t histlen = 0;

void history( std::string s ) {
    ynot::Coord z = ynot::get_window_size();
    histlen = histlen + s.size();
    hist.push_back( s );
    if ( histlen < z.x ) { goto z; }
    while ( histlen >= z.x ) {
        histlen = histlen - hist[0].size();
        hist.erase( hist.begin() );
    }
z:
    for (size_t i = hist.size(); i > 0;)
    {
        i--;
        std::cout << hist[i];
    }
    std::cout << "\n\n";
}

void err ( std::string choice ) {
    printf( "\x1b[H\x1b[2JSearch Word: \n\n"
            "You searched '%s'\n"
            "This word is not in the dictionary\n"
            "Please check spelling and capitalization",
            choice.data() );
}

std::string get() {
    ynot::Coord z = ynot::get_window_size();
    int key = '\b';
    std::string s, b;
s:
    if ( key == '\b' && s.size() != 0 ) {
        printf("%c",key);
        printf("\x1b[K");
        s.pop_back();
    }
    if ( key != '\b' ){
        printf("%c",key);
        s.push_back(char(key));
    }
    if ( s.size() >= z.x - 15 ) { return s; }
a:
    key = ynot::getch_();
    if ( key < 0 || key == '\0' ) {
        goto k;
    }
if ( key != '\x0D' ) { goto s; }
return s;
k:
    key = ynot::getch_();
    goto a;
}

std::string count ( int j ) {
    if ( j == 0 ) { return ""; }
    return "➩ ";
}

size_t home ( size_t index, std::vector<std::string> words, std::vector<size_t> list ) {
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Local variable decleration
    std::vector<std::string>::iterator  it;
    std::string                         choice;
    size_t                              pos = 0;
    ynot::Coord                         z = ynot::get_window_size();
    ///////////////////////////////////////////////////////////////////////////////////////////////
    printf("\x1b[H\x1b[2J");
    ynot::print( ynot::wrap(    "Welcome to the dictionary! Start typing to search for a word\n\n"
                                "Search Word:\n\n"
                                "Use ‘tab’ to auto complete.\n\n"
                                // "Navigate previous searches using ‘Ctrl’ + ‘←’ or ‘Ctrl’ + ‘→’. "
                                // "Return to this page with ‘Ctrl’ + ‘Home’\n\n"
                                "Type ‘readme.txt’ for more\n\n\x1b[3;15H", z.x) );
    // choice = ynot::getline_ac( words );
    // scanf( "%s", choice.data() );
    choice = get();
    if ( choice == "readme.txt" ) {
        system("notepad \"README.md\"");
        return -2;
    }
    it = std::find( words.begin(), words.end(), choice );
    pos = std::distance( words.begin(), it);
    if ( choice != "dictionary" && pos == 1423954 ) { 
        err( choice );
        return -2;
    }
    history( choice );
    return index = list[pos];
}

size_t stand ( size_t index,  std::vector<std::string>  words, std::vector<size_t> list ) {
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Local variable decleration
    std::vector<std::string>::iterator  it;
    std::string                         choice;
    size_t                              pos = 0;
    ynot::Coord                         z = ynot::get_window_size();
    ///////////////////////////////////////////////////////////////////////////////////////////////
    printf("\x1b[HSearch Word:  ");
    choice = get();
    // choice = ynot::getline_ac( words );
    // scanf( "%s", choice.data() );
    if ( choice == "readme.txt" ) {
        system("notepad \"README.md\"");
        return -2;
    }
    it = std::find( words.begin(), words.end(), choice );
    pos = std::distance( words.begin(), it);
    if ( choice != "dictionary" && pos == 1423954 ) { 
        err( choice );
        return -2;
    }
    history( choice );
    return index = list[pos];
}

std::vector<std::string> diction ( std::string Definition ) {
    // make dictionary output 1 string in order to use ynot::paginator
    std::string sent;
    std::vector<std::string> diction;
    std::list<int> objects;
    std::list<int>::iterator it = objects.begin();
    
    rapidjson::Document doc;
    rapidjson::Value senses(rapidjson::kObjectType);

    ynot::Coord z = ynot::get_window_size();
    
    doc.Parse(Definition.c_str());
    senses = doc["senses"];
    
    diction.resize( 1 );
    objects.resize( senses.Size() );

    for ( int k = 0; k < objects.size(); k++) {
        it++;
        *it = k;
    }
    
    it = objects.begin();

    diction[0] = doc["word"].GetString();
    diction[0].append(" (");
    diction[0].append( doc["pos"].GetString() );
    diction[0].append(")\n\n");

    while ( objects.empty() == false ) {
		sent = senses[objects.front()]["id"].GetString();
        if ( senses[objects.front()].HasMember("raw_glosses") ) {
            for ( int j = 0; j < senses[objects.front()]["raw_glosses"].Size(); j++ ) {
                diction.push_back( ynot::wrap( count( j ) + senses[objects.front()]["raw_glosses"][j].GetString(), z.x - ( 5 + ( j + 1) * 8 ), '\n' + std::string( j + 1, '\t' ) ) + '\n' );
            }
        }
        else if ( senses[objects.front()].HasMember("glosses") ) {
            for ( int j = 0; j < senses[objects.front()]["glosses"].Size(); j++ ) {
                diction.push_back( ynot::wrap( count( j ) + senses[objects.front()]["glosses"][j].GetString(), z.x - ( 5 + ( j + 1) * 8 ), '\n' + std::string( j + 1, '\t' ) ) + '\n' );
            }
        }
        else { break; }
        objects.pop_front();
        it = objects.begin();
        sent.push_back('1');
        while ( it != objects.end() ) {
            if ( senses[*it]["id"].GetString() == sent ) {
                if ( senses[*it].HasMember("raw_glosses") ) {
                    int j = senses[*it]["raw_glosses"].Size() - 1;
                    diction.push_back( ynot::wrap( count( j ) + senses[*it]["raw_glosses"][j].GetString(), z.x - ( 5 + ( j + 1) * 8 ), '\n' + std::string( j + 1, '\t' ) ) + '\n' );
                }
                else if ( senses[*it].HasMember("glosses") ) {
                    int j = senses[*it]["glosses"].Size() - 1;
                    diction.push_back( ynot::wrap( count( j ) + senses[*it]["glosses"][j].GetString(), z.x - ( 5 + ( j + 1) * 8 ), '\n' + std::string( j + 1, '\t' ) ) + '\n' );
                }
                else { break; }
                objects.erase(it);
            }
            it++;
        }
        diction.push_back("\n");
    }
    return diction;
}

int main() {
///////////////////////////////////////////////////////////////////////////////////////////////////
// Variable declerations for parsing words.tsv a
    std::ifstream           tsv("word.tsv");  // Index file generated by index.cpp
    std::vector<std::string>
                            list,   // Holds list of words
                            prot;   // Holds raw text that gets sorted
    std::vector<size_t>     words;  // Holds word index
    std::string             word;   // Container for current line
    char                    curr;   // Holds current character

///////////////////////////////////////////////////////////////////////////////////////////////////
// Section for parsing words.tsv and sorting output

    while ( tsv.get( curr ) ) {
        if ( curr == '\n' ) {
            prot.push_back(word);
            word = "";
        }
        word.push_back( curr );
    }

    tsv.close();

    std::sort( prot.begin(), prot.end() );

    for (size_t i = 0; i < prot.size(); i++) {
        list.push_back( prot[i].substr( 1, prot[i].find( '\t' ) - 1 ) );
        words.push_back( std::stoull( prot[i].substr( prot[i].find_last_of( '\t' ), prot[i].size() ) ) );
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declerations for Dictionary parsing

    std::ifstream               file("kaikki.org-dictionary-English.jsonl");  // BIG json FILE 
    std::string                 Definition; // Holds json file
    std::list<int>              line;       // List of word senses
    std::list<int>::iterator    it;         // Iterator for list
    std::vector<std::string>    book;       // Holds return of diction()
    size_t                      index = 0;  // Index of defintion location in BIG FILE
    int                         k;          // Counter for dictionary print size
    ynot::Coord                 z;          // Variable for window size                                

///////////////////////////////////////////////////////////////////////////////////////////////////
// Section for parsing location of a word in the BIG FILE.

    index = home( index, list, words );

start:                                      // Indicates the start of program loop

    if ( index == -1 ) { return 0; }

    if ( index == -2 ) { goto stnd; }

    if ( line.empty() == false ) {
        for ( int i = 0; line.size() != 0; i++) {
            line.pop_front(); 
        }
    }
    
    z = ynot::get_window_size();
    Definition = "";
    k = 0;

    printf("\x1b[H\x1b[2J\n\n");

    history(" > ");

    file.seekg(index);
        
    while ( file.get(curr) ) {
        if (curr == '\n') { break; }
        Definition.push_back(curr);
    }

    book = diction( Definition );

///////////////////////////////////////////////////////////////////////////////////////////////////
// This section is for displaying the results of the dictionary pull

    line.push_back(4);

    for ( int i = 0; i < book.size(); i++) {
        line.push_back( std::count_if( book[i].begin(), book[i].end(), IsNewline ) );
    }

    Display:
    
    it = line.begin();

    for ( int i = 0; i < 1; i++ ) {
        k = k + *it;
        it++;
        if ( k + 1 >= z.y ) 
            { break; }
        ynot::print(book[i]);
    }

    for ( int i = 1; i < book.size(); i++ ) {
        k = k + *it;
        it++;
        if ( k + 1 >= z.y ) 
            { break; }
        ynot::print(book[i]);
    }

stnd:

    index = 0;
           
    index = stand( index, list, words );
    
    goto start;

    // return 0; @ line 180
}
