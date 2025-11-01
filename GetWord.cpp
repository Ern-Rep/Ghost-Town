#include <fstream>
#include <string>
#include <list>
#include <ynot\utf8except.cpp>
#include <ynot\utf8conv.cpp>
#include <ynot\terminal.cpp>
#include <ynot\str.cpp>
#include <rapidjson\document.h>

std::string count ( int j ) {
    if ( j == 0 ) { return ""; }
    return "➩ ";
}

unsigned long long int home( unsigned long long int index ) {
    ynot::Coord z = ynot::get_window_size();
    ynot::print( ynot::wrap( "Welcome to the dictionary! Start typing to search for a word\n\nSearch Word: \n\nUse ‘tab’ to cycle through the auto complete.\n\nNavigate previous searches using ‘Ctrl’ + ‘←’ or ‘Ctrl’ + ‘→’. Return to this page with ‘Ctrl’ + ‘Home’\n\nType ‘readme.txt’ for more\n\n\x1b[3;14H", z.x) );
    scanf( "%llu",&index);
    return index;
}

unsigned long long int stand ( unsigned long long int index ) {
    printf("Search Word: ");
    scanf( "%llu",&index);
    return index;
}

int main() {

    std::string Definition, // holds json file
                sent,
                tab = "    ";
    std::list<int> objects;
    std::list<int>::iterator it = objects.begin();

    std::ifstream file("kaikki.org-dictionary-English.jsonl"); //BIG FILE

    rapidjson::Document doc;
    rapidjson::Value senses(rapidjson::kObjectType);

    /////////////////////////////////////////////////////////
    // Section for parsing location of a word in the BIG FILE.

    char curr = 0;

    unsigned long long int index = 0; // index of defintion location in BIG FILE, eventially will be called on by user.

    index = home( index );

    start:

    printf("\x1b[H\x1b[J\n\n");

    file.seekg(index);

    while ( file.get(curr) ) {
        if (curr == '\n') { break; }
        Definition.push_back(curr);
    }

    doc.Parse(Definition.c_str());
    senses = doc["senses"];

    //  everything above here works well
    /////////////////////////////////////////////////////////
    // This is where the issues start, we have to parse the json for the definitions.


    for ( int i = 0; i < senses.Size(); i++) { objects.push_back(i); }

    ynot::Coord z = ynot::get_window_size();

    std::cout << doc["word"].GetString() << " (" << doc["pos"].GetString() << ")\n\n";

    while ( objects.empty() == false ) {
		sent = senses[objects.front()]["id"].GetString();
		if ( sent.back() != '1' ) {
			if ( senses[objects.front()].HasMember("raw_glosses") ) {
				for ( int j = 0; j < senses[objects.front()]["raw_glosses"].Size(); j++ ) {
                    ynot::print( ynot::wrap( count( j ) + senses[objects.front()]["raw_glosses"][j].GetString(), z.x - ( 5 + ( j + 1) * 8 ), '\n' + std::string( j + 1, '\t' ) ) + '\n' );
                }
			}
			else if ( senses[objects.front()].HasMember("raw_glosses") ) {
				for ( int j = 0; j < senses[objects.front()]["glosses"].Size(); j++ ) {
                    ynot::print( ynot::wrap( count( j ) + senses[objects.front()]["glosses"][j].GetString(), z.x - ( 5 + ( j + 1) * 8 ), '\n' + std::string( j + 1, '\t' ) ) + '\n' );
                }
			}
            else { break; }
			objects.pop_front();
			it = objects.begin();
		}
		sent.push_back('1');
        while ( it != objects.end() ) {
            if ( senses[*it]["id"].GetString() == sent ) {
                if ( senses[*it].HasMember("raw_glosses") ) {
                    int j = senses[*it]["raw_glosses"].Size() - 1;
                    ynot::print( ynot::wrap( count( j ) + senses[*it]["raw_glosses"][j].GetString(), z.x - ( 5 + ( j + 1) * 8 ), '\n' + std::string( j + 1, '\t' ) ) + '\n' );                    
                }
                else if ( senses[*it].HasMember("glosses") ) {
                    int j = senses[*it]["glosses"].Size() - 1;
                    ynot::print( ynot::wrap( count( j ) + senses[*it]["glosses"][j].GetString(), z.x - ( 5 + ( j + 1) * 8 ), '\n' + std::string( j + 1, '\t' ) ) + '\n' );
                }
                else { break; }
                objects.erase(it);
            }
            it++;
        }
        printf("\n");
    }
    printf("\x1b[H");
    index = stand( index );
    
    if ( index != -1 )  { goto start; }
    
    return 0;
}
