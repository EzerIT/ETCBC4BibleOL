/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#include <cassert>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>

#include "verb_class.hpp"
#include "util.hpp"
#include "rapidcsv/src/rapidcsv.h"

using namespace std;

static map<string, set<string>> verb_classes_heb, verb_classes_aram;

// Fields in the csv file:
#define F_SECTION              0    //  Section
#define F_FREQ_RANK            1    //  Frequency rank
#define F_OCCUR                2    //  Occurrences
#define F_ABS_ORDER            3    //  absolut alphabetic order
#define F_LEXEME_IN_LEX        4    //  Lexeme in lexicon
#define F_VOC_LEXEME           5    //  Vocalized lexeme
#define F_CS_SG_FORM           6    //  cs.sg form
#define F_PL_ABS_FORM          7    //  pl_abs form
#define F_VC1                  8    //  Verbal class 1st cons
#define F_VC2                  9    //  Verbal class 2nd cons
#define F_VC3                 10    //  Verbal class 3rd cons
#define F_VCO                 11    //  Verbal class other
#define F_LIST_FORMULA        12    //  Verbal class list with formula
#define F_LIST_NO_FORMULA     13    //  Verbal class list without formula
#define F_PART_OF_SPEECH      14    //  Part of speech
#define F_STEM                15    //  Verbal stems
#define F_CUR_GLOSS           16    //  Current glosses
#define F_SEM_DOMAIN          17    //  Semantic domain
#define F_VOC_LEX_MIT_P       18    //  Vocalized lexeme as in Mitchel (print edition)
#define F_VOC_LEX_MIT_L       19    //  Vocalized lexeme as in Mitchel (Logos version)
#define F_MIT_SEC             20    //  Mitchel Section
#define F_RATIONALE           21    //  Rational for change
#define F_MIT_NON_VERB_GLOSS  22    //  Mitchel non-verb gloss
#define F_NEW_GLOSS           23    //  New glosses (verbs:prioritized displayed meaning of verbs) without excel formula (ready for export)
#define F_NEW_GLOSS_EXCEL     24    //  New glosses (verbs:prioritized displayed meaning of verbs) with excel formula
#define F_MEANING_FIXED       25    //  meaning fixed
#define F_QAL                 26    //  Qal=1
#define F_NIFAL               27    //  Nifal=2
#define F_PIEL                28    //  Piel=3
#define F_PUAL                29    //  Pual=4
#define F_HITPAEL             30    //  Hitpael=5
#define F_HIFIL               31    //  Hifil=6
#define F_HOFAL               32    //  Hofal=7
#define F_HISHTAFAL           33    //  Hishtafal=8
#define F_PAS_QAL             34    //  Passive qal=9
#define F_ETPAAL              35    //  Etpaal=10
#define F_NITPAEL             36    //  Nitpael=11
#define F_HOTPAAL             37    //  Hotpaal=12
#define F_TIFAL               38    //  Tifal=13
#define F_HITPOAL             39    //  Hitpoal=14
#define F_POAL                40    //  Poal=15
#define F_POEL                41    //  Poel=16
#define F_BHVB                42    //  BHVB
#define F_BHVB_SUBSECTION     43    //  Biblical Hebrew Vocabulary Builde subsection
#define F_BHVB_OCCUR          44    //  Occurrences
#define F_OLD_BHVB_SECTION    45    //  Old BHVB section
#define F_OLD_BHVB_SUBSECTION 46    //  Old BHVB subsection
#define F_MAX                 47


// is_hebrew is true for Hebrew and false for Aramaic
void build_newgloss(bool is_hebrew) {
    string lexfilename{is_hebrew ? "ETCBC4-frequency4.07_progression-heb.csv" : "ETCBC4-frequency4.04_progression-aram.csv"};

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

            if (is_hebrew)
                verb_classes_heb[v[F_LEXEME_IN_LEX]] = move(s);
            else
                verb_classes_aram[v[F_LEXEME_IN_LEX]] = move(s);
        }
    }
}

set<string>& verbclasses_lookup(bool is_hebrew, const string &lex)
{
    if (is_hebrew) {
        string lex2;

        // Fix for newly added lexemes
        if (lex=="XWH=[")
            lex2 = "XWH[";
        else if (lex=="XJL==[")
            lex2 = "XJL[";
        else
            lex2 = lex;

        assert(verb_classes_heb.count(lex2)>0);
        return verb_classes_heb[lex2];
    }
    else {
        assert(verb_classes_aram.count(lex)>0);
        return verb_classes_aram[lex];
    }
}

