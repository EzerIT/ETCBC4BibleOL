/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#include <deque>
#include <map>
#include <string>
#include <iostream>

#include "util.hpp"
#include "translit_handler.hpp"
#include "hebrew_transliterator.hpp"


using namespace std;

struct translit_strings {
    string g_word_translit;
    string g_word_nopunct_translit;
    string g_suffix_translit;
    string g_prs_translit;
    string g_vbs_translit;
    string g_pfm_translit;
    string g_vbe_translit;
    string g_nme_translit;
    string g_uvf_translit;
    string g_voc_lex_translit;
    string qere_translit;
};


class translit_handler : public handler {
  public:
    translit_handler();
    
    virtual void list_features(set<string>&) override;
    virtual void prepare_object(map<string,string>&) override;
    virtual void finish_prepare() override;
    virtual string define_features() override;
    virtual string update_object(const map<string,string>&) override;

  private:
    // Transliteration context. We need five words (two before and two after the current one)
    deque<map<string,string>> words;
    map<int, translit_strings> transliterations; // Indexed by self

    void chk_o(const map<string,string> * fmapp);
};




shared_ptr<handler> make_translit_handler()
{
    return shared_ptr<handler>{new translit_handler};
}


translit_handler::translit_handler()
{
    // Start by providing two empty words of context
    words.emplace_back(map<string,string>{{"g_word_utf8",""},{"g_suffix_utf8",""}});
    words.emplace_back(map<string,string>{{"g_word_utf8",""},{"g_suffix_utf8",""}});
}

void translit_handler::list_features(set<string>& req)
{
    initialize_translit_rules();

    req.insert({"g_prs_utf8", "g_vbs_utf8", "g_pfm_utf8",    "g_vbe_utf8",  "g_nme_utf8",     
                "g_uvf_utf8", "g_word",     "g_suffix_utf8", "g_word_utf8", "g_voc_lex_utf8", 
                "qere_utf8",  "lex",        "sp",            "ps",          "gn",
                "nu",         "vs",         "vt"});
}



void translit_handler::chk_o(const map<string,string> * fmapp)
{
    if (!fmapp)
        // Running from finish_prepare(). Add final empty word of context
        words.emplace_back(map<string,string>{{"g_word_utf8",""},{"g_suffix_utf8",""}});
    else 
        words.emplace_back(*fmapp);
        
    if (words.size()==5) {
        // Now we can transliterate the middle word

        bool ketiv = words[2].at("g_word").find('*')!=string::npos;

        translit_strings translits;

        translits.g_suffix_translit = suffix_transliterate(words[2].at("g_suffix_utf8"));
        words[2]["g_suffix_translit"] = translits.g_suffix_translit; // used by transliterate()
        
        translits.g_word_translit = transliterate(words[0].at("g_word_utf8")+words[0].at("g_suffix_utf8")+
                                                words[1].at("g_word_utf8")+words[1].at("g_suffix_utf8"),

                                                words[2].at(ketiv ? "g_word_cons_utf8" : "g_word_utf8"),

                                                words[2].at("g_suffix_utf8")+
                                                words[3].at("g_word_utf8")+words[3].at("g_suffix_utf8")+
                                                words[4].at("g_word_utf8")+words[4].at("g_suffix_utf8"),

                                                words[2],

                                                ketiv, true);

        translits.g_prs_translit = transliterate("", words[2].at("g_prs_utf8"), "", words[2], false, false);
        translits.g_vbs_translit = transliterate("", words[2].at("g_vbs_utf8"), "", words[2], false, false);
        translits.g_pfm_translit = transliterate("", words[2].at("g_pfm_utf8"), "", words[2], false, false);
        translits.g_vbe_translit = transliterate("", words[2].at("g_vbe_utf8"), "", words[2], false, false);
        translits.g_nme_translit = transliterate("", words[2].at("g_nme_utf8"), "", words[2], false, false);
        translits.g_uvf_translit = transliterate("", words[2].at("g_uvf_utf8"), "", words[2], false, false);
        
        translits.g_voc_lex_translit = transliterate("", words[2].at("g_voc_lex_utf8"), "", words[2], false, false);

        if (words[2].count("qere_utf8")>0)
            translits.qere_translit = transliterate("", words[2].at("qere_utf8"), "", words[2], false, false);
        
        char last_char = translits.g_word_translit.back();
        string last_2_char = translits.g_word_translit.length()>=2 ? translits.g_word_translit.substr(translits.g_word_translit.length()-2) : "" ;

        if (last_char=='|' || last_char=='-' || last_char==' ')
            translits.g_word_nopunct_translit = translits.g_word_translit.substr(0, translits.g_word_translit.length()-1);
        else if (last_2_char=="\u05be")
            translits.g_word_nopunct_translit = translits.g_word_translit.substr(0, translits.g_word_translit.length()-2);
        else
            translits.g_word_nopunct_translit = translits.g_word_translit;

        if (translits.g_word_nopunct_translit == "HÎʔ")
            translits.g_word_nopunct_translit = "hîʔ";

        transliterations[stol(words[2].at("self"))] = translits;

        words.pop_front();
    }
}

void translit_handler::prepare_object(map<string,string>& fmap)
{
    chk_o(&fmap);
}


void translit_handler::finish_prepare()
{
    chk_o(nullptr);
    chk_o(nullptr);
}



string translit_handler::define_features()
{
    return
        "    ADD g_word_translit : string DEFAULT \"\";\n"
        "    ADD g_word_nopunct_translit : string DEFAULT \"\";\n"
        "    ADD g_suffix_translit : string DEFAULT \"\";\n"
        "    ADD g_prs_translit : string DEFAULT \"\";\n"
        "    ADD g_vbs_translit : string DEFAULT \"\";\n"
        "    ADD g_pfm_translit : string DEFAULT \"\";\n"
        "    ADD g_vbe_translit : string DEFAULT \"\";\n"
        "    ADD g_nme_translit : string DEFAULT \"\";\n"
        "    ADD g_uvf_translit : string DEFAULT \"\";\n"
        "    ADD g_voc_lex_translit : string DEFAULT \"\";\n"
        "    ADD qere_translit : string DEFAULT \"\";\n";
}


string translit_handler::update_object(const map<string,string>& fmap)
{
    const translit_strings& translits = transliterations.at(stoi(fmap.at("self")));

    return
        "    g_word_translit := \""           + translits.g_word_translit           + "\";\n" 
        "    g_word_nopunct_translit := \""   + translits.g_word_nopunct_translit   + "\";\n" 
        "    g_suffix_translit := \""         + translits.g_suffix_translit         + "\";\n" 
        "    g_prs_translit := \""            + translits.g_prs_translit            + "\";\n" 
        "    g_vbs_translit := \""            + translits.g_vbs_translit            + "\";\n" 
        "    g_pfm_translit := \""            + translits.g_pfm_translit            + "\";\n" 
        "    g_vbe_translit := \""            + translits.g_vbe_translit            + "\";\n" 
        "    g_nme_translit := \""            + translits.g_nme_translit            + "\";\n" 
        "    g_uvf_translit := \""            + translits.g_uvf_translit            + "\";\n" 
        "    g_voc_lex_translit := \""        + translits.g_voc_lex_translit        + "\";\n" 
        "    qere_translit := \""             + translits.qere_translit             + "\";\n"; 
}
