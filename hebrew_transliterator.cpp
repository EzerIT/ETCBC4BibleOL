/* Copyright © 2017 Nicolai Winther-Nielsen and Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <pcrecpp.h>
#include <iostream>
#include <iomanip>

#include "hebrew_transliterator.hpp"
#include "util.hpp"

#ifdef TEST_TRANSLIT
#include <fstream>
#endif

using namespace std;


namespace {
    // Characters and symbols
    const string PUNCT_EXT_BELOW = "\u0323";  // Should be \u05c5, but BHS4 uses \u0323 instead

    const string GERESH          = "\u059c";

    const string SHEWA           = "\u05b0";
    const string HATAPH_SEGOL    = "\u05b1";
    const string HATAPH_PATAH    = "\u05b2";
    const string HATAPH_QAMETS   = "\u05b3";
    const string HIRIQ           = "\u05b4";
    const string TSERE           = "\u05b5";
    const string SEGOL           = "\u05b6";
    const string PATAH           = "\u05b7";
    const string QAMETS          = "\u05b8";
    const string HOLAM           = "\u05b9";
    const string QIBBUTS         = "\u05bb";

    const string DAGESH          = "\u05bc";
    const string METEG           = "\u05bd";
    const string MAQAF           = "\u05be";
    const string RAFE            = "\u05bf";
    const string PASEQ           = "\u05c0";
    const string SHINDOT         = "\u05c1";
    const string SINDOT          = "\u05c2";
    const string SOF_PASUQ       = "\u05c3";
    const string PUNCT_EXT_ABOVE = "\u05c4";
    const string NUN_INVERSUM    = "\u05c6";

    const string ALEPH           = "\u05d0";
    const string BET             = "\u05d1";
    const string GIMEL           = "\u05d2";
    const string DALET           = "\u05d3";
    const string HE              = "\u05d4";
    const string WAW             = "\u05d5";
    const string ZAYIN           = "\u05d6";
    const string HETH            = "\u05d7";
    const string TET             = "\u05d8";
    const string YOD             = "\u05d9";
    const string FINAL_KAPH      = "\u05da";
    const string KAPH            = "\u05db";
    const string LAMED           = "\u05dc";
    const string FINAL_MEM       = "\u05dd";
    const string MEM             = "\u05de";
    const string FINAL_NUN       = "\u05df";
    const string NUN             = "\u05e0";
    const string SAMEK           = "\u05e1";
    const string AYIN            = "\u05e2";
    const string FINAL_PE        = "\u05e3";
    const string PE              = "\u05e4";
    const string FINAL_TSADE     = "\u05e5";
    const string TSADE           = "\u05e6";
    const string QOPH            = "\u05e7";
    const string RESH            = "\u05e8";
    const string SHIN            = "\u05e9";
    const string TAV             = "\u05ea";

    // Regular expressions
    const string vowels       = "([\u05b4-\u05bb]|("+WAW+DAGESH+"))";
    const string longvow      = "(["+TSERE+HOLAM+"])";
    const string consND       = "[\u05d0-\u05ea]["+SHINDOT+SINDOT+"]?";
    const string optdagesh    = DAGESH+"?";
    const string optrafe      = RAFE+"?";
    const string cons         = consND+optdagesh;
    const string consNotAleph = "[\u05d1-\u05ea]["+SHINDOT+SINDOT+"]?" + optdagesh;
    const string cantils      = "[\u0591-\u05af\u05bd]+";
    const string optcantils   = "[\u0591-\u05af\u05bd]*";
    const string wordend      = "($|[ "+MAQAF+SOF_PASUQ+"])"; // "\\b" does not work in Hebrew
    const string wordstart    = "(^|[ "+MAQAF+SOF_PASUQ+"]|^" + GERESH + ")"; // "\\b" does not work in Hebrew

    const string ORIG = "ORIG"; // Special value to indicate no translation

    // Output symbols (transliterations) in lower case
    const string A_BAR                   = "\u0101";
    const string A_SUBSCRIPT             = "\u2090";
    const string A_SUPERSCRIPT           = "\u1d43";
    const string E_BAR                   = "\u0113";
    const string E_CARET                 = "\u00ea";
    const string E_SUPERSCRIPT           = "\u1d49";
    const string E_SUPERSCRIPT_BACKWARDS = "\u1d4a";
    const string H_DOT                   = "\u1e25";
    const string H_SUPERSCRIPT           = "\u02b0";
    const string I_CARET                 = "\u00ee";
    const string O_BAR                   = "\u014d";
    const string O_CARET                 = "\u00f4";
    const string O_SUPERSCRIPT           = "\u1d52";
    const string S_ACUTE                 = "\u015b";
    const string S_CARON                 = "\u0161";
    const string S_DOT                   = "\u1e63";
    const string T_DOT                   = "\u1e6d";
    const string U_CARET                 = "\u00fb";
    const string Y_SUPERSCRIPT           = "\u02b8";
    const string HOOK_LEFT               = "\u0294";
    const string HOOK_LEFT_SUPERSCRIPT   = "\u02c0";
    const string HOOK_RIGHT              = "\u0295";

    // Output symbols (transliterations) in upper case for ketiv
    const string A_BAR_UC                   = "\u0100";
    const string A_SUBSCRIPT_UC             = "\u2090"; // Same as lower case
    const string A_SUPERSCRIPT_UC           = "\u1d43"; // Same as lower case
    const string E_BAR_UC                   = "\u0112";
    const string E_CARET_UC                 = "\u00ca";
    const string E_SUPERSCRIPT_UC           = "\u1d49"; // Same as lower case
    const string E_SUPERSCRIPT_BACKWARDS_UC = "\u1d4a"; // Same as lower case
    const string H_DOT_UC                   = "\u1e24";
    const string H_SUPERSCRIPT_UC           = "\u02b0"; // Same as lower case
    const string I_CARET_UC                 = "\u00ce";
    const string O_BAR_UC                   = "\u014c";
    const string O_CARET_UC                 = "\u00d4";
    const string O_SUPERSCRIPT_UC           = "\u1d52"; // Same as lower case
    const string S_ACUTE_UC                 = "\u015a";
    const string S_CARON_UC                 = "\u0160";
    const string S_DOT_UC                   = "\u1e62";
    const string T_DOT_UC                   = "\u1e6c";
    const string U_CARET_UC                 = "\u00db";
    const string Y_SUPERSCRIPT_UC           = "\u02b8"; // Same as lower case
    const string HOOK_LEFT_UC               = "\u0294"; // Same as lower case
    const string HOOK_LEFT_SUPERSCRIPT_UC   = "\u02c0"; // Same as lower case
    const string HOOK_RIGHT_UC              = "\u0295"; // Same as lower case
}


class rule {
  public:
    rule(const string& precedent, const string& original, const string& succeedent, const string& replacement,
         const string& replacement_uc,const map<string, string>& attribs = map<string,string>{}, bool require_cantil=false)
        : m_precedent{precedent},
          m_original{original},
          m_succeedent{succeedent},
          m_replacement{replacement},
          m_replacement_uc{replacement_uc},
          m_attribs{attribs},
          m_require_cantil{require_cantil},
          m_beforepattern{precedent + "$", pcrecpp::UTF8()},
          m_afterpattern{"^" + original + succeedent, pcrecpp::UTF8()},
          m_consumepattern{"^(" + original + ")", pcrecpp::UTF8()}
        {}

    rule(const string& precedent, const string& original, const string& succeedent, const string& replacement,
         const string& replacement_uc, bool require_cantil)
        : rule(precedent, original, succeedent, replacement, replacement_uc, map<string,string>{}, require_cantil)
        {}
    

    string replacement(bool uc) const {
        return uc ? m_replacement_uc : m_replacement;
    }

    string original() const {
        return m_original;
    }

    bool matches_start_of(const string& after) const {
        return m_afterpattern.PartialMatch(after);
    }

    bool matches_end_of(const string& before) const {
        return m_beforepattern.PartialMatch(before);
    }

    bool matches_attributes(const map<string,string>& input_attributes) const {
        for (const pair<string,string>& ia : m_attribs)
            if (input_attributes.count(ia.first)==0 || input_attributes.at(ia.first)!=ia.second)
                return false;

        return true;
    }

    bool matches(const string& before, const string& after, const map<string, string>& input_attributes, bool cantil_available) const {
        if (m_require_cantil && !cantil_available)
            return false;
        
        return matches_end_of(before) && matches_start_of(after) &&
            matches_attributes(input_attributes);
    }

    string consumes(const string& after) const {
        string s;
        m_consumepattern.PartialMatch(after, &s);
        return s;
    }

    string to_string() const {
        return m_precedent + m_original + m_succeedent + " (" + std::to_string(m_attribs.size()) + ") => " +
            m_precedent + m_replacement + m_succeedent;
    }

  private:
    string m_precedent, m_original, m_succeedent, m_replacement, m_replacement_uc;
    bool m_require_cantil; // Use this rule only if cantillation marks are available
    pcrecpp::RE m_beforepattern, m_afterpattern, m_consumepattern;
    map<string, string> m_attribs;

};

class verbrule {
  public:
    verbrule(const string& original, const string& replacement1, const string& replacement2)
        : m_original{original},
          m_replacement1{replacement1},
          m_replacement2{replacement2},
          m_pattern{"^" + original, pcrecpp::UTF8()}
        {}
    
    string replacement(bool dagesh) const {
        return dagesh ? m_replacement1 : m_replacement2;
    }

    bool matches(const string& after) const {
        return m_pattern.PartialMatch(after);
    }

    string consumes() const {
        return m_original;
    }

    string to_string() const {
        return m_original + " => " + m_replacement1 + "/" + m_replacement2;
    }

  private:
    string m_original, m_replacement1, m_replacement2;
    pcrecpp::RE m_pattern;
};



static vector<rule> rules;
static vector<verbrule> verbrules;

void initialize_translit_rules()
{
    // Some verbs require knowledge of the word forms to correctly transliterate qamets and shewa.
    // The following vector holds these forms
    vector<map<string, string>> attribvec {
        {
            // Example: Monad 1844, "GEN 04,11", "פָּצְתָ֣ה", "pāṣᵊtāʰ";
            {"vs", "qal"},
            {"ps", "p3"},
            {"gn", "f"},
            {"nu", "sg"},
            {"vt", "perf"},
        },
        {
            // Example: Monad 1764, "GEN 04,06", "נָפְל֥וּ", "nāfᵊlû";
            {"vs", "qal"},
            {"ps", "p3"},
            {"nu", "pl"},
            {"vt", "perf"},
        },
        {
            // Example: Monad 277809, "EZE 26,21", "תִמָּצְאִ֥י", "timmāṣᵊʔî";
            {"vs", "nif"},
            {"ps", "p2"},
            {"gn", "f"},
            {"nu", "sg"},
            {"vt", "impf"},
        },
        {
            // Example: Monad 8332, "GEN 18,29", "יִמָּצְא֥וּן", "yimmāṣᵊʔûn";
            {"vs", "nif"},
            {"ps", "p3"},
            {"gn", "m"},
            {"nu", "pl"},
            {"vt", "impf"},
        },
        {
            // Example: Monad 63976, "LEV 19,12", "תִשָּׁבְע֥וּ", "tiššāvᵊʕû";
            {"vs", "nif"},
            {"ps", "p2"},
            {"gn", "m"},
            {"nu", "pl"},
            {"vt", "impf"},
        },
        {
            // Example: Monad 237770, "JER 06,08", "הִוָּסְרִי֙", "hiwwāsᵊrî";
            {"vs", "nif"},
            {"ps", "p2"},
            {"gn", "f"},
            {"nu", "sg"},
            {"vt", "impv"},
        },
        {
            // Example: Monad 27795,  "GEN 49,02", "הִקָּבְצ֥וּ", "hiqqāvᵊṣû";
            {"vs", "nif"},
            {"ps", "p2"},
            {"gn", "m"},
            {"nu", "pl"},
            {"vt", "impv"},
        },
        {
            // Example: Monad 8881,   "GEN 19,20", "אִמָּלְטָ֨ה", "ʔimmālᵊṭāʰ";
            {"vs", "nif"},
            {"ps", "p1"},
            {"nu", "sg"},
            {"vt", "impf"},
        },
        {
            // Example: Monad 227312, "JES 43,26", "נִשָּׁפְטָ֖ה", "niššāfᵊṭāʰ";
            {"vs", "nif"},
            {"ps", "p1"},
            {"nu", "pl"},
            {"vt", "impf"},
        },
    };

    vector<map<string, string>> attribvec2 {
        {
            // Example: Monad 256432, "JER 39,04", "רָ֠אָם", "rāʔām";
            {"g_prs_utf8", QAMETS+FINAL_MEM},
        },
    };
            

    // Rules for special characters:
    rules.emplace_back("", "[ 0-9()]+",  "", ORIG, ORIG);
    rules.emplace_back("", cantils,      "", "",   ""  );
    rules.emplace_back("", MAQAF,        "", "-",  "-" );
    rules.emplace_back("", RAFE,         "", "-",  "-" );
    rules.emplace_back("", PASEQ,        "", "|",  "|" );
    rules.emplace_back("", SOF_PASUQ,    "", ":",  ":" );
    rules.emplace_back("", NUN_INVERSUM, "", "",   ""  );

    // Rules for shewa:
    rules.emplace_back(wordstart,                    SHEWA, "",                 E_SUPERSCRIPT_BACKWARDS, E_SUPERSCRIPT_BACKWARDS_UC ); // Only relevant in graphical_pron_suffix
    rules.emplace_back(wordstart+cons+optcantils,    SHEWA, "",                 E_SUPERSCRIPT_BACKWARDS, E_SUPERSCRIPT_BACKWARDS_UC );
    rules.emplace_back("",                           SHEWA, wordend,            "",                      ""                         );
    rules.emplace_back(cons,                         SHEWA, cons+SHEWA+wordend, "",                      ""                         );
    rules.emplace_back(longvow+cons+optcantils,      SHEWA, "",                 E_SUPERSCRIPT_BACKWARDS, E_SUPERSCRIPT_BACKWARDS_UC );
    rules.emplace_back(SHEWA+cons+optcantils,        SHEWA, "",                 E_SUPERSCRIPT_BACKWARDS, E_SUPERSCRIPT_BACKWARDS_UC );

    for (const map<string, string>& mp : attribvec)
        rules.emplace_back(QAMETS+cons+optcantils,   SHEWA, "",                 E_SUPERSCRIPT_BACKWARDS, E_SUPERSCRIPT_BACKWARDS_UC, mp);

    rules.emplace_back(METEG+cons,                   SHEWA, "",                 E_SUPERSCRIPT_BACKWARDS, E_SUPERSCRIPT_BACKWARDS_UC );
    rules.emplace_back(QAMETS+cantils+cons,          SHEWA, "",                 E_SUPERSCRIPT_BACKWARDS, E_SUPERSCRIPT_BACKWARDS_UC );
    rules.emplace_back(cantils+optdagesh+QAMETS+cons,SHEWA, "",                 E_SUPERSCRIPT_BACKWARDS, E_SUPERSCRIPT_BACKWARDS_UC );
    rules.emplace_back(consND+optcantils+DAGESH,     SHEWA, "",                 E_SUPERSCRIPT_BACKWARDS, E_SUPERSCRIPT_BACKWARDS_UC );
    rules.emplace_back(consND+optrafe,               SHEWA, "",                 "",                      ""                         ); // Rafe only relevant in Is 22:10
    rules.emplace_back("["+QAMETS+PATAH+"]"+optcantils, SHEWA, "",                 "y",                     "Y"                        ); // only in yᵊrûšālaymāʰ/yᵊrûšālāymāʰ


    // Rules for qamets:
    rules.emplace_back("",                QAMETS+optcantils+HE,        wordend,      A_BAR+H_SUPERSCRIPT, A_BAR_UC+H_SUPERSCRIPT_UC );
    rules.emplace_back("",                QAMETS+optcantils+HE+DAGESH, wordend,      A_BAR+"h",           A_BAR_UC+"H"              );
    rules.emplace_back("",                QAMETS+optcantils+YOD,       WAW,          A_BAR+Y_SUPERSCRIPT, A_BAR_UC+Y_SUPERSCRIPT_UC );
    rules.emplace_back("",                QAMETS+optcantils+YOD,       wordend,      A_BAR+Y_SUPERSCRIPT, A_BAR_UC+Y_SUPERSCRIPT_UC );

    for (const map<string, string>& mp : attribvec)
        rules.emplace_back("",            QAMETS,                      cons+SHEWA,           A_BAR, A_BAR_UC, mp   );
                                                                                              
    rules.emplace_back(cantils+optdagesh, QAMETS,                      "",                   A_BAR, A_BAR_UC       );
    rules.emplace_back("",                QAMETS,                      cons+SHEWA,           "o",   "O",      true );
    for (const map<string, string>& mp : attribvec2)
        rules.emplace_back("",            QAMETS,                      consNotAleph+wordend, A_BAR, A_BAR_UC, mp   );
    rules.emplace_back("",                QAMETS,                      consNotAleph+wordend, "o",   "O",      true );
    rules.emplace_back("",                QAMETS,                      "",                   A_BAR, A_BAR_UC       );


    // Rules for remaining vowels:
    rules.emplace_back("", HATAPH_SEGOL,               "",                    E_SUPERSCRIPT,          E_SUPERSCRIPT_UC          );
    rules.emplace_back("", HATAPH_PATAH,               "",                    A_SUPERSCRIPT,          A_SUPERSCRIPT_UC          );
    rules.emplace_back("", HATAPH_QAMETS,              "",                    O_SUPERSCRIPT,          O_SUPERSCRIPT_UC          );

    rules.emplace_back("", HIRIQ+optcantils,           YOD+optcantils+DAGESH, "i",                    "I"                       );
    rules.emplace_back("", HIRIQ+optcantils+YOD,       "",                    I_CARET,                I_CARET_UC                );
    rules.emplace_back("", HIRIQ,                      "",                    "i",                    "I"                       );

    rules.emplace_back("", TSERE+optcantils+HE,        wordend,               E_BAR+H_SUPERSCRIPT,    E_BAR_UC+H_SUPERSCRIPT_UC );
    rules.emplace_back("", TSERE+optcantils+HE+DAGESH, wordend,               E_BAR+"h",              E_BAR_UC+"H"              );
    rules.emplace_back("", TSERE+optcantils+YOD,       "",                    E_CARET,                E_CARET_UC                );
    rules.emplace_back("", TSERE,                      "",                    E_BAR,                  E_BAR_UC                  );

    rules.emplace_back("", SEGOL+optcantils+HE,        wordend,               "e"+H_SUPERSCRIPT,      "E"+H_SUPERSCRIPT_UC      );
    rules.emplace_back("", SEGOL+optcantils+HE+DAGESH, wordend,               "eh",                   "EH"                      );
    rules.emplace_back("", SEGOL+optcantils+YOD,       "",                    "e"+Y_SUPERSCRIPT,      "E"+Y_SUPERSCRIPT_UC      );
    rules.emplace_back("", SEGOL,                      "",                    "e",                    "E"                       );

    rules.emplace_back("", AYIN+optcantils+PATAH,      wordend,               A_SUBSCRIPT+HOOK_RIGHT, A_SUBSCRIPT_UC+HOOK_RIGHT_UC);
    rules.emplace_back("", HETH+optcantils+PATAH,      wordend,               A_SUBSCRIPT+H_DOT,      A_SUBSCRIPT_UC+H_DOT_UC   );
    rules.emplace_back("", PATAH,                      "",                    "a",                    "A"                       );

    rules.emplace_back("", HOLAM+optcantils+WAW,       "",                    O_CARET,                O_CARET_UC                );
    rules.emplace_back("", HOLAM+optcantils+HE,        wordend,               O_BAR+H_SUPERSCRIPT,    O_BAR_UC+H_SUPERSCRIPT_UC );
    rules.emplace_back("", HOLAM+optcantils+HE+DAGESH, wordend,               O_BAR+"h",              O_BAR_UC+"H"              ); // Apparently never occurs
    rules.emplace_back("", HOLAM,                      "",                    O_BAR,                  O_BAR_UC                  );

    rules.emplace_back("", QIBBUTS,                    "",                    "u",                    "U"                    );


    // Rules for consonants:
    rules.emplace_back("",                        ALEPH+optcantils+DAGESH,        "",     HOOK_LEFT+HOOK_LEFT,   HOOK_LEFT_UC+HOOK_LEFT_UC); //rare
    rules.emplace_back(vowels+optcantils,         ALEPH,                          vowels, HOOK_LEFT,             HOOK_LEFT_UC             );
    rules.emplace_back(vowels+optcantils,         ALEPH,                          "",     HOOK_LEFT_SUPERSCRIPT, HOOK_LEFT_SUPERSCRIPT_UC );
    rules.emplace_back("",                        ALEPH,                          "",     HOOK_LEFT,             HOOK_LEFT_UC             );

    rules.emplace_back(vowels+optcantils,         BET+optcantils+DAGESH,          "",     "bb",                  "BB"                  );
    rules.emplace_back("",                        BET+optcantils+DAGESH,          "",     "b",                   "B"                   );
    rules.emplace_back("",                        BET,                            "",     "v",                   "V"                   );

    rules.emplace_back(vowels+optcantils,         GIMEL+optcantils+DAGESH,        "",     "gg",                  "GG"                  );
    rules.emplace_back("",                        GIMEL+optcantils+DAGESH,        "",     "g",                   "G"                   );
    rules.emplace_back("",                        GIMEL,                          "",     "g",                   "G"                   );

    rules.emplace_back(vowels+optcantils,         DALET+optcantils+DAGESH,        "",     "dd",                  "DD"                  );
    rules.emplace_back("",                        DALET+optcantils+DAGESH,        "",     "d",                   "D"                   );
    rules.emplace_back("",                        DALET,                          "",     "d",                   "D"                   );

    rules.emplace_back("",                        HE+optcantils+DAGESH,           "",     "h",                   "H"                   );
    rules.emplace_back("",                        HE,                             "",     "h",                   "H"                   );

    rules.emplace_back(cons+optcantils+optdagesh, WAW+optcantils+DAGESH,          "",     U_CARET,               U_CARET_UC            );
    rules.emplace_back(wordstart,                 WAW+optcantils+DAGESH,          "",     U_CARET,               U_CARET_UC            );
    rules.emplace_back("",                        WAW+optcantils+DAGESH,          "",     "ww",                  "WW"                  );
    rules.emplace_back("",                        WAW,                            "",     "w",                   "W"                   );

    rules.emplace_back("",                        ZAYIN+optcantils+DAGESH,        "",     "zz",                  "ZZ"                  );
    rules.emplace_back("",                        ZAYIN,                          "",     "z",                   "Z"                   );

    rules.emplace_back("",                        HETH,                           "",     H_DOT,                 H_DOT_UC              );

    rules.emplace_back("",                        TET+optcantils+DAGESH,          "",     T_DOT+T_DOT,           T_DOT_UC+T_DOT_UC     );
    rules.emplace_back("",                        TET,                            "",     T_DOT,                 T_DOT_UC              );

    rules.emplace_back("",                        YOD+optcantils+DAGESH,          "",     "yy",                  "YY"                  );
    rules.emplace_back("",                        YOD,                            "",     "y",                   "Y"                   );

    rules.emplace_back(vowels+optcantils,         FINAL_KAPH+optcantils+DAGESH,   "",     "kk",                  "KK"                  );
    rules.emplace_back("",                        FINAL_KAPH+optcantils+DAGESH,   "",     "k",                   "K"                   );
    rules.emplace_back("",                        FINAL_KAPH,                     "",     "x",                   "X"                   );

    rules.emplace_back(vowels+optcantils,         KAPH+optcantils+DAGESH,         "",     "kk",                  "KK"                  );
    rules.emplace_back("",                        KAPH+optcantils+DAGESH,         "",     "k",                   "K"                   );
    rules.emplace_back("",                        KAPH,                           "",     "x",                   "X"                   );

    rules.emplace_back("",                        LAMED+optcantils+DAGESH,        "",     "ll",                  "LL"                  );
    rules.emplace_back("",                        LAMED,                          "",     "l",                   "L"                   );

    rules.emplace_back("",                        MEM+optcantils+DAGESH,          "",     "mm",                  "MM"                  );
    rules.emplace_back("",                        MEM,                            "",     "m",                   "M"                   );
    rules.emplace_back("",                        FINAL_MEM+optcantils+DAGESH,    "",     "mm",                  "MM"                  ); // For g_lex_utf8
    rules.emplace_back("",                        FINAL_MEM,                      "",     "m",                   "M"                   );

    rules.emplace_back("",                        NUN+optcantils+DAGESH,          "",     "nn",                  "NN"                  );
    rules.emplace_back("",                        FINAL_NUN+optcantils+DAGESH,    "",     "nn",                  "NN"                  ); // For verbal ending
    rules.emplace_back("",                        FINAL_NUN,                      "",     "n",                   "N"                   );
    rules.emplace_back("",                        NUN,                            "",     "n",                   "N"                   );

    rules.emplace_back("",                        SAMEK+optcantils+DAGESH,        "",     "ss",                  "SS"                  );
    rules.emplace_back("",                        SAMEK,                          "",     "s",                   "S"                   );

    rules.emplace_back("",                        AYIN,                           "",     HOOK_RIGHT,            HOOK_RIGHT_UC         );

    rules.emplace_back("",                        FINAL_PE+optcantils+DAGESH,     "",     "p",                   "P"                   );
    rules.emplace_back("",                        FINAL_PE,                       "",     "f",                   "F"                   );

    rules.emplace_back(vowels+optcantils,         PE+optcantils+DAGESH,           "",     "pp",                  "PP"                  );
    rules.emplace_back("",                        PE+optcantils+DAGESH,           "",     "p",                   "P"                   );
    rules.emplace_back("",                        PE,                             "",     "f",                   "F"                   );

    rules.emplace_back("",                        FINAL_TSADE+optcantils+DAGESH,        "",     S_DOT+S_DOT,           S_DOT_UC+S_DOT_UC     ); // For g_lex_utf8
    rules.emplace_back("",                        FINAL_TSADE,                    "",     S_DOT,                 S_DOT_UC              );
    rules.emplace_back("",                        TSADE+optcantils+DAGESH,        "",     S_DOT+S_DOT,           S_DOT_UC+S_DOT_UC     );
    rules.emplace_back("",                        TSADE,                          "",     S_DOT,                 S_DOT_UC              );

    rules.emplace_back("",                        QOPH+optcantils+DAGESH,         "",     "qq",                  "QQ"                  );
    rules.emplace_back("",                        QOPH,                           "",     "q",                   "Q"                   );

    rules.emplace_back("",                        RESH+optcantils+DAGESH,         "",     "rr",                  "RR"                  );
    rules.emplace_back("",                        RESH,                           "",     "r",                   "R"                   );

    rules.emplace_back("",                        SHIN+SHINDOT+optcantils+DAGESH, "",     S_CARON+S_CARON,       S_CARON_UC+S_CARON_UC );
    rules.emplace_back("",                        SHIN+SHINDOT,                   "",     S_CARON,               S_CARON_UC            );
    rules.emplace_back("",                        SHIN+SINDOT+optcantils+DAGESH,  "",     S_ACUTE+S_ACUTE,       S_ACUTE_UC+S_ACUTE_UC );
    rules.emplace_back("",                        SHIN+SINDOT,                    "",     S_ACUTE,               S_ACUTE_UC            );
    rules.emplace_back("",                        SHIN,                           "",     "s",                   "S"                   );

    rules.emplace_back(vowels+optcantils,         TAV+optcantils+DAGESH,          "",     "tt",                  "TT"                  );
    rules.emplace_back("",                        TAV+optcantils+DAGESH,          "",     "t",                   "T"                   );
    rules.emplace_back("",                        TAV,                            "",     "t",                   "T"                   );
}

void initialize_translit_verbrules()
{
    // First, for the few verbs, (such as גָּבַהּ) that (erroneously?) have vowels and mapiq/dagesh in their lexeme
    verbrules.emplace_back(QAMETS, "", "");
    verbrules.emplace_back(PATAH,  "", "");
    verbrules.emplace_back(DAGESH, "", ""); // Same as mapiq
    verbrules.emplace_back(SHEWA,  "", "");
    
    // Second, replace all consonants with upper case Latin characters
    verbrules.emplace_back(ALEPH,        HOOK_LEFT_UC,  HOOK_LEFT_UC );
    verbrules.emplace_back(BET,          "B",           "V"          );
    verbrules.emplace_back(GIMEL,        "G",           "G"          );
    verbrules.emplace_back(DALET,        "D",           "D"          );
    verbrules.emplace_back(HE,           "H",           "H"          );
    verbrules.emplace_back(WAW,          "W",           "W"          );
    verbrules.emplace_back(ZAYIN,        "Z",           "Z"          );
    verbrules.emplace_back(HETH,         H_DOT_UC,      H_DOT_UC     );
    verbrules.emplace_back(TET,          T_DOT_UC,      T_DOT_UC     );
    verbrules.emplace_back(YOD,          "Y",           "Y"          );
    verbrules.emplace_back(FINAL_KAPH,   "K",           "X"          );
    verbrules.emplace_back(KAPH,         "K",           "X"          );
    verbrules.emplace_back(LAMED,        "L",           "L"          );
    verbrules.emplace_back(FINAL_MEM,    "M",           "M"          );
    verbrules.emplace_back(MEM,          "M",           "M"          );
    verbrules.emplace_back(FINAL_NUN,    "N",           "N"          );
    verbrules.emplace_back(NUN,          "N",           "N"          );
    verbrules.emplace_back(SAMEK,        "S",           "S"          );
    verbrules.emplace_back(AYIN,         HOOK_RIGHT_UC, HOOK_RIGHT_UC);
    verbrules.emplace_back(FINAL_PE,     "P",           "F"          );
    verbrules.emplace_back(PE,           "P",           "F"          );
    verbrules.emplace_back(FINAL_TSADE,  S_DOT_UC,      S_DOT_UC     );
    verbrules.emplace_back(TSADE,        S_DOT_UC,      S_DOT_UC     );
    verbrules.emplace_back(QOPH,         "Q",           "Q"          );
    verbrules.emplace_back(RESH,         "R",           "R"          );
    verbrules.emplace_back(SHIN+SHINDOT, S_CARON_UC,    S_CARON_UC   );
    verbrules.emplace_back(SHIN+SINDOT,  S_ACUTE_UC,    S_ACUTE_UC   );
    verbrules.emplace_back(SHIN,         "S",           "S"          );
    verbrules.emplace_back(TAV,          "T",           "T"          );
}


/*
 * The actual transliteration function.
 * Pass in a morpheme, together with the text which precedes and
 * follows it, as this text may affect the transliteration.
 */
string transliterate(string preceding_text, string input,
                     string following_text, const map<string, string>& input_attributes, bool uc, bool do_qere_perpetuum, bool cantil_available) {
    // std::cout << "Starting transliterate(\"" << preceding_text << "\", input=\"" << input << "\", following_text = " << following_text << ", input_attributes<>..., uc, do_qere_perpetuum, cantil_available): self = " << input_attributes.at("self") << std::endl;
    
    // Remove puncta extraordinaria
    replace_string_in_place(input,          PUNCT_EXT_ABOVE, "");
    replace_string_in_place(input,          PUNCT_EXT_BELOW, "");
    replace_string_in_place(preceding_text, PUNCT_EXT_ABOVE, "");
    replace_string_in_place(preceding_text, PUNCT_EXT_BELOW, "");
    replace_string_in_place(following_text, PUNCT_EXT_ABOVE, "");
    replace_string_in_place(following_text, PUNCT_EXT_BELOW, "");

    if (do_qere_perpetuum) {
        // Qere perpetuum - start
        if (input_attributes.at("g_word_cons_utf8") == HE+WAW+ALEPH &&
            input_attributes.at("sp") == "prps" &&
            input_attributes.at("ps") == "p3" &&
            input_attributes.at("gn") == "f" &&
            input_attributes.at("nu") == "sg") {
            return "HÎ"+HOOK_LEFT;
        }

        if (input_attributes.at("lex") == "JHWH/")
            return "YHWH";
        // Qere perpetuum - end
    }

    string output;

    for (string::size_type pos = 0; pos < input.length(); /* pos += consumed.length() */) {
        string before = preceding_text + input.substr(0, pos);
        string after = input.substr(pos) + following_text;
        string consumed, result;

        for (rule rl : rules) {
            if (rl.matches(before, after, input_attributes, cantil_available)) {
                // cout << "before=>" << before << "< after=>" << after << "< rl=>" << rl.to_string() << endl;
                consumed = rl.consumes(after);
                result = rl.replacement(uc);
                if (result == ORIG)
                    result = consumed;
                break;
            }
        }

        if (consumed.empty()) {
            // nothing matched, this is probably bad!
            // copy and consume one character and try again
            consumed = after.substr(0, 1);
            result = consumed;
            cerr << "Input = " << input << endl;
#ifdef TEST_TRANSLIT
            cerr << "No transliteration for character '" << hex << setw(2) << (unsigned int)(unsigned char)result[0] << "'" << endl;
#else
            cerr << "No transliteration for character '" << hex << setw(2) << (unsigned int)(unsigned char)result[0] << "' self="
                 << dec << input_attributes.at("self") << endl;
#endif
        }

        output += result;
        pos += consumed.length();
    }

    // std::cout << "Done! Output = '" << output << "'" << std::endl;
    return output;
}



// Transliterate a verb.
string transliterate_verb_lex(const string& input)
{
    string output;

    for (string::size_type pos = 0; pos < input.length(); /* pos += consumed.length() */) {
        string after = input.substr(pos);
        string consumed, result;

        for (verbrule rl : verbrules) {
            if (rl.matches(after)) {
                consumed = rl.consumes();
                result = rl.replacement(pos==0);
                break;
            }
        }

        if (consumed.empty()) {
            // nothing matched, this is probably bad!
            // copy and consume one character and try again
            consumed = after.substr(0, 1);
            result = consumed;
            cerr << "Input = " << input << endl;
            cerr << "No verb transliteration for character '" << hex << setw(2) << (unsigned int)(unsigned char)result[0] << "'" << endl;
        }

        output += result;
        pos += consumed.length();
    }
    return output;
}


string suffix_transliterate(const string& heb_suffix)
{
    static map<string,string> suffixmap {
        { " ",                     " "  }, // Space 
        { "",                      ""   }, // No space
        { "\u05be",                "-"  }, // Maqaf
        { "\u05c0 ",               " "  }, // Paseq
        { "\u05c3 ",               ": " }, // Sof pasuq 
        { "\u05c3 \u05c6 ",        ": " }, // Sof pasuq, nun hafukha
        { "\u05c3 \u05c6 \u05e1 ", ": " }, // Sof pasuq, nun hafukha, samekh
        { "\u05c3 \u05c6 \u05e4 ", ": " }, // Sof pasuq, nun hafukha, pe
        { "\u05c3 \u05e1 ",        ": " }, // Sof pasuq, samekh
        { "\u05c3 \u05e4 ",        ": " }, // Sof pasuq, pe
        { " \u05e1 ",              " "  }, // Samekh 
        { " \u05e4 ",              " "  }, // pe
	{ "\u05c3 \u05e0 \u05e1 ", ": " }, // Sof pasuq, nun, samekh
	{ "\u05c3 \u05e0 \u05e4 ", ": " }, // Sof pasuq, nun, pe
	{ " \u05c0 ",              " "  }, // Paseq
    };

    if (!suffixmap.contains(heb_suffix)) {
	std::cerr << "ERROR: heb_suffix = '" << heb_suffix << "' is not present in suffixmap.\nPlease add it in hebrew_transliterator.cpp." << std::endl;
	// The at() below will cause an error.
    }

    return suffixmap.at(heb_suffix);
}

#ifdef TEST_TRANSLIT

// Test strings

vector<string> heb {
    "יִרְמְיָהוּ",
    "יִשְׁרְצ֣וּ",
    "קָ֣רָא",
    "יִּקְרָא־",
    "שָׂא־",
    "נָּא־",
    "רָ֠אָם"
};

vector<string> hebverb {
    "ברא",
    "בברא",
"",
"גָּבַהּ",
"יָלַהּ",
"כָּמַהּ",
"מָהַהּ",
"נָגַהּ",
"תְּוַהּ",
"תָּמַהּ",
};

int main()
{
    initialize_translit_rules();
    initialize_translit_verbrules();

    ofstream mlout{"transout.txt"};

#if 0
    for (const string& h : heb) {
        map<string, string> att {
            {"g_suffix_translit"," "},
            {"gn","f"},
            {"lexeme_plain_utf8","ב"},
            {"vs","qal"},
            {"nu","sg"},
            {"sp","verb"},
            {"ps","p3"},
            {"vt","perf"},
            {"g_word_cons_utf8",""},
            {"lex",""}
        };

        for (unsigned char c : h)
            cout << hex << setw(2) << setfill('0') << (unsigned int)c << " ";
        cout << "\n" << dec;
        
        string lat {transliterate("",h,"",att,false,false,true)};

        mlout << lat << endl;
        cout << h << "  :  " << lat << endl;
    }
#endif

#if 0
    for (const string& h : hebverb) {
        for (unsigned char c : h)
            cout << hex << setw(2) << setfill('0') << (unsigned int)c << " ";
        cout << "\n" << dec;
        
        string lat {transliterate_verb_lex(h)};

        mlout << lat << endl;
        cout << h << "  :  " << lat << endl;
    }
#endif

#if 1
    string h = "רְב֗וּ";
    string pref = "וּ";
    
    string lat {transliterate(pref,h,"",map<string, string>{},false,false,true)};
    cout << lat << "\n";
#endif
}
#endif
