// Copyright © 2023 Claus Tøndering (claus@tondering.dk)
// Released under an MIT License.
//
// This code generates a CSV file with information about what monads are in which object


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
#include <memory>
#include <cassert>

#include "emdros_iterators.hpp"

using namespace std;

int main(int argc, char **argv)
{
    if (argc!=3) {
        cerr << "Usage: make_sequence_csv <database name> <output file>\n";
        return 1;
    }

    std::string output_filename = argv[2];

    ofstream ofile{output_filename.c_str()};
    if (!ofile) {
        cerr << "Cannot open " << output_filename << endl;
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

    // Fetch information about all words

    cout << "Handling word" << endl;

    string mql_request { "SELECT ALL OBJECTS WHERE [verse get book,chapter,verse [word GET g_word_utf8,monad_num]] GO" };

    if (!EE.executeString(mql_request, bResult, false, true))
        return 1;

    if (!EE.isSheaf()) {
        cerr << "ERROR: Result is not sheaf\n";
        return 1;
    }

    map<monad_m, map<string, string>> feature_maps;  // Maps monad number to feature=>value map

    for (StrawOk s_outer : SheafOk{EE.getSheaf()}) {
        for (const MatchedObject mo_outer : s_outer) {
  
            for (StrawOk s_inner : SheafOk{mo_outer.getSheaf()}) {
                for (const MatchedObject mo_inner : s_inner) {
                    map<string, string> fmap;

                    fmap["book"] = mo_outer.getFeatureAsString(0);
                    fmap["chapter"] = mo_outer.getFeatureAsString(1);
                    fmap["verse"] = mo_outer.getFeatureAsString(2);
                    fmap["text"] = mo_inner.getFeatureAsString(0);


                    SetOfMonads som = mo_inner.getMonads();
                    assert(som.first() == som.last());
                    assert(som.first() == mo_inner.getFeatureAsLong(1));

                    feature_maps[som.first()] = fmap;
                }
            }
        }
    }

    // Fetch information about other object types

    for (const string& object_type : { "subphrase", "phrase_atom", "phrase", "clause_atom", "clause", "sentence_atom", "sentence" }) {
        cout << "Handling " << object_type << endl;

        string mql_request2 { "SELECT ALL OBJECTS WHERE [" + object_type + " GET sequence_number] GO" };

        if (!EE.executeString(mql_request2, bResult, false, true))
            return 1;
        
        if (!EE.isSheaf()) {
            cerr << "ERROR: Result is not sheaf\n";
            return 1;
        }

        for (StrawOk s_outer : SheafOk{EE.getSheaf()}) {
            for (const MatchedObject mo : s_outer) {
                SetOfMonads som = mo.getMonads();

                monad_m low = som.first();
                monad_m high = som.last();

                for (auto m=low; m<=high; ++m)
                    feature_maps[m][object_type] = mo.getFeatureAsString(0);
            }
        }
    }


    // Generate output
    vector<string> headings {"text","book","chapter","verse","subphrase", "phrase_atom", "phrase", "clause_atom", "clause", "sentence_atom", "sentence"};

    ofile << "\"monad\",";
    for (const string& h : headings)
        ofile << "\"" << h << "\"" << (h=="sentence" ? "\n" : ",");

    for (auto fmap : feature_maps) {
        ofile << fmap.first << ",";
        for (const string& ix : headings)
            ofile << (ix=="text" || ix=="book" ? "\"" : "") << fmap.second[ix] << (ix=="text" || ix=="book" ? "\"" : "") << (ix=="sentence" ? "\n" : ",");
    }
}
