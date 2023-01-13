/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#include <cassert>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include <functional>

#include "util.hpp"
#include "frequency_handler.hpp"
#include "rapidcsv/src/rapidcsv.h"

using namespace std;



class frequency_handler : public handler {
  public:
    virtual void list_features(set<string>&) override;
    virtual void prepare_object(map<string,string>&) override;
    virtual void finish_prepare() override;
    virtual string define_features() override;
    virtual string update_object(const map<string,string>&) override;

  private:
    struct Frequency_data {
        map<string,int> total;        // Maps lexeme to frequency
        map<string,int> rank;         // Maps lexeme to rank
        map<string,string> fa_order;  // Maps lexeme to frequency+alphabetic order (technically an
                                      // integer, but we don't want to convert string => int => string
    } freq_info[2];  // Index 0 is Hebrew, index 1 is Aramaic
};

shared_ptr<handler> make_frequency_handler()
{
    return shared_ptr<handler>{new frequency_handler};
}



void frequency_handler::list_features(set<string>& req)
{
    req.insert({"language", "lex"});
}


void frequency_handler::prepare_object(map<string,string>& fmap)
{
    int lang_index = fmap.at("language")=="Hebrew" ? 0 : 1;

    ++freq_info[lang_index].total[fmap.at("lex")];
}


void frequency_handler::finish_prepare()
{
    for (int lang_index=0; lang_index<2; ++lang_index) {
        Frequency_data& fd = freq_info[lang_index];

        multimap<int,string,greater<int>> freq2lex;      // Maps frequency to lexeme, sorted by descending frequency
        
        for (const auto& me : fd.total)
            freq2lex.insert(make_pair(me.second, me.first));

        // Use frequencies to generate frequency ranks

        int rank = 1;		// Current rank
		int last_freq = 0;	// Last frequency inserted
		int last_rank = 0;	// Rank of last frequency inserted

        for (const auto& p : freq2lex) {
            // p.first is frequency, p.second is lexeme
            if (p.first==last_freq)
                fd.rank[p.second] = last_rank;
            else {
                fd.rank[p.second] = rank;
                last_freq = p.first;
                last_rank = rank;
            }
            ++rank;
        }

        assert(fd.total.size()==fd.rank.size());
    }


    // Add fa_order from spreadsheet

    // Fields in the csv file:
    constexpr int F_SECTION		  = 0; //  Section
	constexpr int F_FA_ORDER	  = 1; //  Frequency+alphabetica order
	constexpr int F_OCCUR		  = 2; //  Occurrences
	constexpr int F_ABS_ORDER	  = 3; //  absolut alphabetic order
	constexpr int F_LEXEME_IN_LEX = 4; //  Lexeme in lexicon
    
    for (int lang_index=0; lang_index<2; ++lang_index) { // lang_index is 0 for Hebrew, 1 for Aramaic
        string lexfilename{lang_index==0 ? "ETCBC4-frequency4.04_progression-heb.csv" : "ETCBC4-frequency4.04_progression-aram.csv"};
        rapidcsv::Document lexfile;

        try {
            lexfile.Load(lexfilename);
        }
        catch (const ios_base::failure& e) {
            cerr << e.what() << "\n";
            cerr << "Cannot open " << lexfilename << "\n";
            exit(1);
        }

        for (size_t rix=0; rix<lexfile.GetRowCount(); ++rix) {
            vector<string> v = lexfile.GetRow<string>(rix);

            cout << "lexfilename = '" << lexfilename << "' +++ " << v[F_LEXEME_IN_LEX] << " +++ " << freq_info[lang_index].rank.count(v[F_LEXEME_IN_LEX]) << " +++" << std::endl;
            
            assert(freq_info[lang_index].rank.count(v[F_LEXEME_IN_LEX])>0);
            assert(freq_info[lang_index].fa_order.count(v[F_LEXEME_IN_LEX])==0);
            
            freq_info[lang_index].fa_order[v[F_LEXEME_IN_LEX]] = v[F_FA_ORDER];
        }
    }
}


string frequency_handler::define_features()
{
	return
        "    ADD lexeme_occurrences : integer DEFAULT 0;\n"
        "    ADD frequency_rank : integer DEFAULT 99999;\n"
        "    ADD fa_order : integer DEFAULT 99999;\n";
}
 
string frequency_handler::update_object(const map<string,string>& fmap)
{
    if (fmap.at("language")=="Hebrew")
        return
            "    lexeme_occurrences := " + to_string(freq_info[0].total.at(fmap.at("lex")))  + ";\n"
            "    frequency_rank := "     + to_string(freq_info[0].rank.at(fmap.at("lex")))   + ";\n"
            "    fa_order := "           + freq_info[0].fa_order.at(fmap.at("lex"))          + ";\n";
    else
        return
            "    lexeme_occurrences := " + to_string(freq_info[1].total.at(fmap.at("lex")))  + ";\n"
            "    frequency_rank := "     + to_string(freq_info[1].rank.at(fmap.at("lex")))   + ";\n"
            "    fa_order := "           + freq_info[1].fa_order.at(fmap.at("lex"))          + ";\n";
}
