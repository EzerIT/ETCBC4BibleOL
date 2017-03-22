/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#include <emdros/emdfdb.h>
#include <emdros/emdros_environment.h>
#include <emdros/mql_sheaf.h>
#include <emdros/emdf_value.h>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <vector>

#include "emdros_iterators.hpp"

using namespace std;



static map<string, set<pair<string,string>>> lexeme2word; // lexeme => words
static map<string, set<pair<string,string>>> lexeme2suffix; // lexeme => suffixes
static set<string> allLexemes;
static set<pair<string,string>> allWords;
static set<pair<string,string>> allSuffixes;


int main(int argc, char **argv)
{
    if (argc!=3) {
        cerr << "usage: worddb <emdrosfile> <sqlfile>" << endl;
        return 1;
    }
        

    EmdrosEnv EE{kOKConsole,
            kCSUTF8,
            "localhost",
            "",
            "",
            argv[1],
            kSQLite3};

    bool bResult{false};

    string mql_request{"SELECT ALL OBJECTS WHERE [word GET "
            "lex,g_word_nocant_utf8,g_word_nopunct_translit,g_prs_utf8,g_prs_translit] GO"};
    if (!EE.executeString(mql_request, bResult, false, true))
        return 1;

    if (!EE.isSheaf()) {
        cerr << "ERROR: Result is not sheaf\n";
        return 1;
    }


    map<int, map<string, string>> feature_maps; // Maps id_d to feature=>value map

    for (StrawOk str : SheafOk{EE.getSheaf()}) {
        for (const MatchedObject mo : str) {
            string lex {mo.getFeatureAsString(0)};
            string g_word_nocant_utf8 {mo.getFeatureAsString(1)};
            string g_word_nopunct_translit {mo.getFeatureAsString(2)};
            string g_prs_utf8 {mo.getFeatureAsString(3)};
            string g_prs_translit {mo.getFeatureAsString(4)};

            allLexemes.insert(lex);

            auto g_word_pair = make_pair(g_word_nocant_utf8,g_word_nopunct_translit);
            allWords.insert(g_word_pair);
            lexeme2word[lex].insert(g_word_pair);

            auto suf_pair = make_pair(g_prs_utf8,g_prs_translit);
            allSuffixes.insert(suf_pair);
            lexeme2suffix[lex].insert(suf_pair);
        }
    }


    // cout << "lex: " << allLexemes.size() << endl;
    // cout << "txt: " << allWords.size() << endl;
    // cout << "suf: " << allSuffixes.size() << endl;
    // cout << "l2w: " << lexeme2word.size() << endl;
    // cout << "l2s: " << lexeme2suffix.size() << endl;


    /* Generate SQL */

    ofstream sqlfile{argv[2]};

    sqlfile << "CREATE TABLE lexemes (id integer primary key, lex text);" << endl;
    sqlfile << "CREATE TABLE lexsuf (id integer primary key, lexid integer, sufid integer);" << endl;
    sqlfile << "CREATE TABLE lextext (id integer primary key, lexid integer, textid integer);" << endl;
    sqlfile << "CREATE TABLE suffixes (id integer primary key, suffix blob, suffix_translit blob);" << endl;
    sqlfile << "CREATE TABLE texts (id integer primary key, word blob, word_translit blob);" << endl;

    sqlfile << "BEGIN TRANSACTION;" << endl;

    for (const auto& s : allLexemes)
        sqlfile << "INSERT INTO lexemes VALUES(NULL,'" << s << "');" << endl;


    for (const auto& s : allWords)
        sqlfile << "INSERT INTO texts VALUES(NULL,'" << s.first << "','" << s.second << "');" << endl;


    for (const auto& s : allSuffixes)
        sqlfile << "INSERT INTO suffixes VALUES(NULL,'" << s.first << "','" << s.second << "');" << endl;
    sqlfile << "COMMIT;" << endl;


    sqlfile << "CREATE INDEX ixlexemes on lexemes(lex);" << endl;
    sqlfile << "CREATE INDEX ixlexsuf on lexsuf(lexid);" << endl;
    sqlfile << "CREATE INDEX ixlextext on lextext(lexid);" << endl;
    sqlfile << "CREATE INDEX ixsuffixes on suffixes(suffix);" << endl;
    sqlfile << "CREATE INDEX ixsuffixes2 on suffixes(suffix_translit);" << endl;
    sqlfile << "CREATE INDEX ixtexts on texts(word);" << endl;
    sqlfile << "CREATE INDEX ixtexts2 on texts(word_translit);" << endl;

    sqlfile << "BEGIN TRANSACTION;" << endl;
    for (const auto& l2w : lexeme2word)
        for (const auto& s : l2w.second)
            sqlfile << "INSERT INTO lextext VALUES (NULL,"
                "(SELECT ID FROM lexemes WHERE lex='" << l2w.first << "'),"
                "(SELECT ID FROM texts WHERE word='" << s.first << "' AND word_translit='" << s.second << "'));" << endl;

    for (const auto& l2s : lexeme2suffix)
        for (const auto& s : l2s.second)
            sqlfile << "INSERT INTO lexsuf VALUES (NULL,"
                "(SELECT ID FROM lexemes WHERE lex='" << l2s.first << "'),"
                "(SELECT ID FROM suffixes WHERE suffix='" << s.first << "' AND suffix_translit='" << s.second << "'));" << endl;
            
    sqlfile << "COMMIT;" << endl;
}
