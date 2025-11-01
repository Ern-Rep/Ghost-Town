#include <fstream>
#include <string>
#include <rapidjson\document.h>

int main()
{
    std::ifstream file("kaikki.org-dictionary-English.jsonl");
    std::fstream out;

    // json declaration
    rapidjson::Document json;

    // string declarations
    std::string NameOfFile;
    std::string CurrentLine;
    std::string Line2Add;

    // int declarations
    unsigned long long int CharCount = 0;
    unsigned long long int LineLocation = 0;
    unsigned long int cnt = 0;

    // char declarations
    char curr;
    char prev;

    //Loop for extracting text from the JSONL file. Makes new JSON file according to what text is in the file.

    while ( curr != file.end ) {
        prev = curr;
        file.get(curr);
        CharCount++;
        // checks to make sure that newline is formatted correctly;
        if (curr == '\n' && prev == '}' )
        {
            // Pass pointer to current line to json parser
            json.Parse(CurrentLine.c_str());

            // Add to the file according to what character it starts with
            NameOfFile = "./words/word";
            // NameOfFile.push_back(tolower(json["word"].GetString()[0]));
            NameOfFile += (".tsv");

            // Line to add to file
            Line2Add = "";
            Line2Add += (json["word"].GetString());
            Line2Add.push_back(9);
            Line2Add += (json["pos"].GetString());
            Line2Add.push_back(9);
            Line2Add += (std::to_string(LineLocation));
            Line2Add.push_back(9);
            Line2Add += (std::to_string(cnt));
            Line2Add.push_back('\n');

            if ( cnt%1000 == 0 )
                printf("%s", Line2Add.c_str() );
            // Open correct file and add next word to it
                // send itterator to before end of file and append text

            out.open(NameOfFile,std::fstream::app);
            out << Line2Add;
            out.close();

            // clear the current line string
            CurrentLine = "";

            // increment the cnt of current number of words.
            cnt++;

            // increment line character
            LineLocation = CharCount;
        }
        CurrentLine.push_back(curr);
    }

    return 0;
}