/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#include <cassert>
#include <map>
#include <vector>
#include <pcrecpp.h>
#include <iostream>
#include <fstream>
#include <set>

#include "verb_class.hpp"
#include "util.hpp"

using namespace std;
using pcrecpp::RE;
using pcrecpp::StringPiece;

static map<string, set<string>> verb_classes;

// Fields in the csv file:
#define F_SECTION              0    //  Section
#define F_FREQ_RANK            1    //  Frequency rank
#define F_OCCUR                2    //  Occurrences
#define F_ABS_ORDER            3    //  absolut alphabetic order
#define F_LEXEME_IN_LEX        4    //  Lexeme in lexicon
#define F_VOC_LEXEME           5    //  Vocalized lexeme
#define F_VC1                  6    //   verbal class 1st cons
#define F_VC2                  7    //   verbal class 2nd cons
#define F_VC3                  8    //   verbal class 3rd cons
#define F_VCO                  9    //   verbal class other
#define F_PART_OF_SPEECH      10    //  Part of speech
#define F_STEM                11    //  Verbal stems
#define F_CUR_GLOSS           12    //  Current glosses
#define F_VOC_LEX_MIT_P       14    //  Vocalized lexeme as in Mitchel (print edition)
#define F_VOC_LEX_MIT_L       15    //  Vocalized lexeme as in Mitchel (Logos version)
#define F_MIT_SEC             16    //  Mitchel Section
#define F_RATIONALE           17    //  Rational for change
#define F_MIT_NON_VERB_GLOSS  18    //  Mitchel non-verb gloss
#define F_NEW_GLOSS           20    //    New glosses (verbs:prioritized displayed meaning of verbs) without excel formula (ready for export)
#define F_NEW_GLOSS_EXCEL     22    //    New glosses (verbs:prioritized displayed meaning of verbs) with excel formula
#define F_MEANING_FIXED       24    //  meaning fixed
#define F_QAL                 25    //  Qal=1
#define F_NIFAL               27    //  Nifal=2
#define F_PIEL                29    //  Piel=3
#define F_PUAL                31    //  Pual=4
#define F_HITPAEL             33    //  Hitpael=5
#define F_HIFIL               35    //  Hifil=6
#define F_HOFAL               37    //  Hofal=7
#define F_HISHTAFAL           39    //  Hishtafal=8
#define F_PAS_QAL             41    //  Passive qal
#define F_HOTPAAL             43    //  Hotpaal
#define F_ETPAAL              44    //  Etpaal
#define F_NITPAEL             45    //  Nitpael
#define F_TIFAL               46    //   Tifal
#define F_MAX                 47


void build_newgloss() {
    ifstream lexfile{"ETCBC4-frequency3.52_progression.csv"};
    string buf;

    RE entry{"((\"([^\"]+)\")|([^\",]*)),"}; // Relies on a final comma
    
    string x1,x2;
    string quoted;
    string unquoted;

    // Skip header lines
    getline(lexfile,buf);

    int cnt = 1;


    while (getline(lexfile,buf)) {
        ++cnt;
        buf += ",";  // Regex relies on a final comma
        replace_string_in_place(buf, "\"\"", "¤");   // "" => ¤

        StringPiece haystack{buf};

        string ignore1,ignore2;
        string quoted;
        string unquoted;

        vector<string> v;

        while (entry.Consume(&haystack, &ignore1, &ignore2, &quoted, &unquoted)) {
            if (!quoted.empty()) {
                replace_string_in_place(quoted, "¤", "\\\"");  // Extra \ needed when writing to MQL file
                v.push_back(quoted);
            }
            else
                v.push_back(unquoted);
        }

        if (v.size()>F_MAX) {
            cerr << "ERROR: CSV file contains a line with " << v.size() << " fields\n";
            exit(1);
        }

        {
            set<string> s;
            if (!v[F_VC1].empty())
                s.insert(v[F_VC1]);

            if (!v[F_VC2].empty())
                s.insert(v[F_VC2]);

            if (!v[F_VC3].empty())
                s.insert(v[F_VC3]);

            if (!v[F_VCO].empty())
                s.insert(v[F_VCO]);

            verb_classes[v[F_LEXEME_IN_LEX]] = move(s);
        }
    }
}

set<string>& verbclasses_lookup(const string &lex)
{
    string lex2;

    // Fix for newly added lexemes
    if (lex=="XWH=[")
        lex2 = "XWH[";
    else if (lex=="XJL==[")
        lex2 = "XJL[";
    else
        lex2 = lex;

    //cout << "LEX2 " << lex2 << endl;
    assert(verb_classes.count(lex2)>0);
    return verb_classes[lex2];
}

