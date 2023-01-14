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
#include <memory>
#include <cassert>

#include "emdros_iterators.hpp"
#include "frequency_handler.hpp"
#include "verb_stem_handler.hpp"
#include "verb_class_handler.hpp"
#include "strip_handler.hpp"
#include "translit_handler.hpp"
#include "suffix_handler.hpp"
#include "monad_feature_handler.hpp"

using namespace std;



static string build_request(const set<string>& required_features, map<string,int>& simap, const string& book="")
{
//    string s { "SELECT ALL OBJECTS IN {1-11} WHERE [verse GET label [word GET " };  // Only Gen 1:1
//    string s { "SELECT ALL OBJECTS IN {1-1666} WHERE [verse GET label [word GET " };  // Only Gen 1-3
//    string s { "SELECT ALL OBJECTS IN {1-28762} WHERE [verse GET label [word GET " }; // Only Genesis
    string s { "SELECT ALL OBJECTS WHERE [verse "
            + (book=="" ? "" : ("book=" + book))
            + " GET label [word GET " };

    int ix = 0;
    for (const string& feat : required_features) {
        s += feat + ",";
        simap[feat] = ix++;
    }

    s[s.length()-1] = ']'; // Replace final ',' with ']'
    s += "] GO\n";

    return s;
}


vector<string> allbooks {
    "Genesis",
    "Exodus",
    "Leviticus",
    "Numeri",
    "Deuteronomium",
    "Josua",
    "Judices",
    "Samuel_I",
    "Samuel_II",
    "Reges_I",
    "Reges_II",
    "Jesaia",
    "Jeremia",
    "Ezechiel",
    "Hosea",
    "Joel",
    "Amos",
    "Obadia",
    "Jona",
    "Micha",
    "Nahum",
    "Habakuk",
    "Zephania",
    "Haggai",
    "Sacharia",
    "Maleachi",
    "Psalmi",
    "Iob",
    "Proverbia",
    "Ruth",
    "Canticum",
    "Ecclesiastes",
    "Threni",
    "Esther",
    "Daniel",
    "Esra",
    "Nehemia",
    "Chronica_I",
    "Chronica_II",
};


int main(int argc, char **argv)
{
    if (argc<3) {
        cerr << "Usage: emdros_updater <database name> <output file> [<bookname1> <bookname2> ...]\n";
        return 1;
    }

    std::string output_filename = argv[2];

    std::vector<std::string> book_names;

    if (argc == 3) {
	book_names = allbooks;
    } else {
	int arg_index;
	for (arg_index = 3; arg_index < argc; ++arg_index) {
	    book_names.push_back(std::string(argv[arg_index]));
	}
    }

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

    //=======================================================
    // Gather frequency information for entire Old Testament
    //=======================================================

    cout << "Gathering required frequency information for the entire Old Testament..." << endl;

    shared_ptr<handler> freq_hand = make_frequency_handler();
    {
        set<string> required_features{"self"}; // We always need "self"

        freq_hand->list_features(required_features);

        map<string,int> simap;
        string mql_request = build_request(required_features, simap);
        if (!EE.executeString(mql_request, bResult, false, true))
            return 1;

        if (!EE.isSheaf()) {
            cerr << "ERROR: Result is not sheaf\n";
            return 1;
        }


        map<int, map<string, string>> feature_maps; // Maps id_d to feature=>value map

        for (StrawOk s_outer : SheafOk{EE.getSheaf()}) {
            for (const MatchedObject mo_outer : s_outer) {
  
                string verse_label = mo_outer.getFeatureAsString(0);

                for (StrawOk s_inner : SheafOk{mo_outer.getSheaf()}) {
                    for (const MatchedObject mo_inner : s_inner) {
                        map<string, string> fmap;

                        fmap["verse_label"] = verse_label;

                        for (pair<string, int> si : simap)
                            fmap[si.first] = mo_inner.getFeatureAsString(si.second);

                        feature_maps[mo_inner.getID_D()] = fmap;
                    }
                }
            }
        }

        for (auto& fm : feature_maps)
            freq_hand->prepare_object(fm.second);

        freq_hand->finish_prepare();

        // *freq_hand now contains frequencies and frequency ranks for all lexemes
    }
    cout << "Done gatering frequency information for the entire Old Testament!\n" << endl;


    // The remainder is done per book of the OT
    bool has_defined_features = false;

    for (string& book : book_names) {
        // Create handlers. The frequency handler is being reused from above
        // Alas, we cannot use unique_ptr in handlers, because initializer list elemets must be copyable

        cout << "Handling " << book << endl;

        
        vector<shared_ptr<handler>> handlers {
            make_verb_stem_handler(),
            make_verb_class_handler(),
            make_strip_handler(),
            make_translit_handler(),
            make_suffix_handler(),
            make_monad_feature_handler(),
        };

        //==================================================
        // Gather information
        //==================================================

	cout << "Gathering required information for " << book << " ..." << endl;

	cout << "... Building MQL request ... " << flush;
        set<string> required_features{"self"}; // We always need "self"
        freq_hand->list_features(required_features);
        for (const auto& h : handlers)
            h->list_features(required_features);

        map<string,int> simap;
        string mql_request = build_request(required_features, simap, book);
	cout << "Done!" << endl;

	
	cout << "... Executing MQL request ... " << flush;
        if (!EE.executeString(mql_request, bResult, false, true))
            return 1;

        if (!EE.isSheaf()) {
            cerr << "ERROR: Result is not sheaf\n";
            return 1;
        }
	cout << "Done!" << endl;

	cout << "... Harvesting the results from the sheaf ... " << flush;
        map<int, map<string, string>> feature_maps; // Maps id_d to feature=>value map

        for (StrawOk s_outer : SheafOk{EE.getSheaf()}) {
            for (const MatchedObject mo_outer : s_outer) {
  
                string verse_label = mo_outer.getFeatureAsString(0);

                for (StrawOk s_inner : SheafOk{mo_outer.getSheaf()}) {
                    for (const MatchedObject mo_inner : s_inner) {
                        map<string, string> fmap;

                        fmap["verse_label"] = verse_label;

                        for (pair<string, int> si : simap)
                            fmap[si.first] = mo_inner.getFeatureAsString(si.second);
                        
                        // Add "monad" as a feature value
                        SetOfMonads som = mo_inner.getMonads();
                        assert(som.first() == som.last());
                        
                        fmap["monad"] = to_string(som.first());
                        
                        feature_maps[mo_inner.getID_D()] = fmap;
                    }
                }
            }
        }
	cout << "Done!" << endl;
	
	cout << "... Preparing objects for " << book << " ... " << endl;
        int count = 0, fullcount = 0;
        int mapsize = feature_maps.size();
        for (auto& fm : feature_maps) {
	    // cout << "... id_d = " << fm.first << " verse_label = " << fm.second["verse_label"] << " monad = " << fm.second["monad"] << " lex = " << fm.second["lex"] << endl;
	    
            for (const auto& h : handlers) {
		// cout << "handler = " << h->handler_name() << endl;
		
                h->prepare_object(fm.second);
	    }

            ++fullcount;
            if (count++ == 1000) {
                cout << fullcount*100/mapsize << "%\r" << flush;
                count = 0;
            }
        }
        cout << "100%" << endl;  // Just to look pretty

	cout << "Done!" << endl;

	cout << "... Finishing the preparation of objects for " << book << " ... " << flush;
        for (const auto& h : handlers)
            h->finish_prepare();
	cout << "Done!" << endl;

	cout << "Done gathering required information for " << book << ".\n" << endl;
	
        //==================================================
        // Generate MQL
        //==================================================

	cout << "Generating MQL for " << book << " in file '" << output_filename << "' ..." << endl;

	cout << "... Generating MQL by calling pre_create() and define_features() for [word]... " << flush;
        if (!has_defined_features) {
            has_defined_features = true;
        
            ofile << freq_hand->pre_create();
            for (const auto& h : handlers)
                ofile << h->pre_create();

            ofile << "UPDATE OBJECT TYPE [word\n";
            ofile << freq_hand->define_features();
            for (const auto& h : handlers)
                ofile << h->define_features();
            ofile << "] GO\n\n";
        }
	cout << "Done!" << endl;

	
        ofile << "\n// " << book << "\n\n"
              << "BEGIN TRANSACTION GO\n";

        count = 0;

	cout << "... Generating MQL by calling update_object() for [word]... " << flush;
        for (auto& fm : feature_maps) {
            if (count++ == 1000) {
                ofile << "COMMIT TRANSACTION GO\n"
                      << "BEGIN TRANSACTION GO\n";
            
                count = 0;
            }

	    // cout << "    word.id_d = " << fm.first;
            ofile << "UPDATE OBJECT BY ID_DS = " << fm.first << " [word\n";

	    // cout << ", ... calling freq_hand->update_object(), ..." << flush;
	    
            ofile << freq_hand->update_object(fm.second);

	    // cout << ", Done! Now doing other handlers->update_object() ..." << flush;

	    for (const auto& h : handlers)
                ofile << h->update_object(fm.second);
            ofile << "] GO\n";

	    // cout << "Done!" << endl;
	    
        }
	cout << "Done!" << endl;


        ofile << "COMMIT TRANSACTION GO\n";

	cout << "Done generating MQL.\n" << endl;
    }
    ofile << "\nVACUUM DATABASE ANALYZE GO\n";
}
