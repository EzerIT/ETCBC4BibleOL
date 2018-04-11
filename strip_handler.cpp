/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <string>

#include "strip_handler.hpp"
#include "util.hpp"

using namespace std;

class strip_handler : public handler {
  public:
    virtual void list_features(set<string>&) override;
    virtual void prepare_object(map<string,string>& fmap) override;
    virtual string define_features() override;
    virtual string update_object(const map<string,string>&) override;

  private:
    string strip_cant(string text);
    string strip_transscribed_cant(const string& text);
    string strip_vowels(string text);
};

shared_ptr<handler> make_strip_handler()
{
    return shared_ptr<handler>{new strip_handler};
}


void strip_handler::list_features(set<string>& req)
{
    req.insert({"g_lex_utf8", "g_nme_utf8",  "g_pfm_utf8", "g_prs_utf8", "g_vbs_utf8",
                "g_vbe_utf8", "g_word_utf8", "g_uvf_utf8", "lex_utf8",   "g_voc_lex_utf8",
                "g_word",     "g_suffix_utf8" });
}



string strip_handler::strip_cant(string text)
{
    static vector<string> cantillations {
        "\u0590",
        "\u0591",
        "\u0592",
        "\u0593",
        "\u0594",
        "\u0595",
        "\u0596",
        "\u0597",
        "\u0598",
        "\u0599",
        "\u059a",
        "\u059b",
        "\u059c",
        "\u059d",
        "\u059e",
        "\u059f",
        "\u05a0",
        "\u05a1",
        "\u05a2",
        "\u05a3",
        "\u05a4",
        "\u05a5",
        "\u05a6",
        "\u05a7",
        "\u05a8",
        "\u05a9",
        "\u05aa",
        "\u05ab",
        "\u05ac",
        "\u05ad",
        "\u05ae",
        "\u05af",
        "\u05c0",
        "\u05c4",
        "\u0323",  // Lower dots - should be "\u05c5"  -- TODO: Prøv at rette dette til 05c5
    };

    for (const auto& cant : cantillations)
        replace_string_in_place(text, cant, "");
    
    return text;
}

// Strip cantillation marks from text, which is in the transcribed alphabet
string strip_handler::strip_transscribed_cant(const string& text)
{
    // All numeric codes except 35, 75, and 95 (all of which represent a meteg) are removed from text

    string res;  // The string to return
    string code; // The current numeric code
    
    for (auto c : text) {
        if (!code.empty()) {
            // We already have one digit of a two-digit code
            
            if (!isdigit(c)) {
                cerr << "Bad digit sequence in " << text << endl;
                return "";
            }
            code += c;
            if (code=="35" || code=="75" || code=="95")
                res += code;
            code = "";
        }
        else if (isdigit(c))
            code += c;
        else
            res += c;
    }
    return res;
}


string strip_handler::strip_vowels(string text)
{
    static vector<string> vowels {
        "\u05b0",
        "\u05b1",
        "\u05b2",
        "\u05b3",
        "\u05b4",
        "\u05b5",
        "\u05b6",
        "\u05b7",
        "\u05b8",
        "\u05b9",
        "\u05ba",
        "\u05bb",
        "\u05bc",
        "\u05bd",
        "\u05bf",
        "\u05c3", // Not really a vowel
    };

    for (const auto& vow : vowels)
        replace_string_in_place(text, vow, "");
    
    return text;
}


void strip_handler::prepare_object(map<string,string>& fmap)
{
    // These need to be modified for proper final consonant.
    fmap["g_nme_utf8"] = fix_final(fmap["g_nme_utf8"]); 
    fmap["g_prs_utf8"] = fix_final(fmap["g_prs_utf8"]); 
    fmap["g_vbe_utf8"] = fix_final(fmap["g_vbe_utf8"]);

    auto suffix = fmap.at("g_suffix_utf8");
    if (suffix.length()>0) {
        auto x = fix_final(fmap["g_word_utf8"]);
        if (x!=fmap["g_word_utf8"]) {
            cout << "Bad final character corrected: " << fmap["g_word_utf8"] << " " << fmap["verse_label"] << endl;
            fmap["g_word_utf8"] = x;
            fmap["g_word_utf8_modified"] = "y";
        }
    }


    // Fix an error in ETCBC4:

    if (fmap.at("g_voc_lex_utf8")=="\xd7\x99\xd6\xb0\xd7\xa8\xd6\xb4\xd7\x99\xd7\x97\xd6\xb9\xd7\x95\xd6\xbf \xd7\x99\xd6\xb0\xd7\xa8\xd6\xb4\xd7\x97\xd6\xb9\xd7\x95")
        fmap["g_voc_lex_utf8"] = "\xd7\x99\xd6\xb0\xd7\xa8\xd6\xb4\xd7\x97\xd6\xb9\xd7\x95";

    
    fmap["g_lex_cons_utf8"]     = strip_vowels(strip_cant(fmap.at("g_lex_utf8")));
    fmap["g_nme_cons_utf8"]     = strip_vowels(strip_cant(fmap.at("g_nme_utf8")));
    fmap["g_pfm_cons_utf8"]     = strip_vowels(strip_cant(fmap.at("g_pfm_utf8")));
    fmap["g_prs_cons_utf8"]     = strip_vowels(strip_cant(fmap.at("g_prs_utf8")));
    fmap["g_vbs_cons_utf8"]     = strip_vowels(strip_cant(fmap.at("g_vbs_utf8")));
    fmap["g_vbe_cons_utf8"]     = strip_vowels(strip_cant(fmap.at("g_vbe_utf8")));
    fmap["g_uvf_cons_utf8"]     = strip_vowels(strip_cant(fmap.at("g_uvf_utf8")));
    fmap["lex_cons_utf8"]       = strip_vowels(strip_cant(fmap.at("lex_utf8")));
    fmap["g_voc_lex_cons_utf8"] = strip_vowels(strip_cant(fmap.at("g_voc_lex_utf8")));

    string g_word_nocant_utf8   = strip_cant(fmap.at("g_word_utf8"));
    fmap["g_word_nocant_utf8"]  = g_word_nocant_utf8;
    fmap["g_word_cons_utf8"]    = strip_vowels(g_word_nocant_utf8);

    fmap["g_word_nocant"]       = strip_transscribed_cant(fmap.at("g_word"));
}

string strip_handler::define_features()
{
    return
        "    ADD g_lex_cons_utf8 : string DEFAULT \"\";\n"
        "    ADD g_nme_cons_utf8 : string DEFAULT \"\";\n"
        "    ADD g_pfm_cons_utf8 : string DEFAULT \"\";\n"
        "    ADD g_prs_cons_utf8 : string DEFAULT \"\";\n"
        "    ADD g_vbs_cons_utf8 : string DEFAULT \"\";\n"
        "    ADD g_vbe_cons_utf8 : string DEFAULT \"\";\n"
        "    ADD g_uvf_cons_utf8 : string DEFAULT \"\";\n"
        "    ADD lex_cons_utf8 : string DEFAULT \"\";\n"
        "    ADD g_voc_lex_cons_utf8 : string DEFAULT \"\";\n"
        "    ADD g_word_nocant : string DEFAULT \"\";\n"
        "    ADD g_word_nocant_utf8 : string DEFAULT \"\";\n"
        "    ADD g_word_cons_utf8 : string DEFAULT \"\";\n";
}


string strip_handler::update_object(const map<string,string>& fmap)
{
    return
        ((fmap.count("g_word_utf8_modified")>0 && fmap.at("g_word_utf8_modified")=="y")
         ? "    g_word_utf8 := \""          + fmap.at("g_word_utf8")          + "\";\n"
         : "") +
        
        // Fix an error in ETCBC4:
        (fmap.at("g_voc_lex_utf8")=="\xd7\x99\xd6\xb0\xd7\xa8\xd6\xb4\xd7\x97\xd6\xb9\xd7\x95"
         ? "    g_voc_lex_utf8 := \"" +   fmap.at("g_voc_lex_utf8")      + "\";\n"
         : "") +
        
        "    g_nme_utf8 := \""          + fmap.at("g_nme_utf8")          + "\";\n"
        "    g_prs_utf8 := \""          + fmap.at("g_prs_utf8")          + "\";\n"
        "    g_vbe_utf8 := \""          + fmap.at("g_vbe_utf8")          + "\";\n"
        "    g_lex_cons_utf8 := \""     + fmap.at("g_lex_cons_utf8")     + "\";\n"
        "    g_nme_cons_utf8 := \""     + fmap.at("g_nme_cons_utf8")     + "\";\n"
        "    g_pfm_cons_utf8 := \""     + fmap.at("g_pfm_cons_utf8")     + "\";\n"
        "    g_prs_cons_utf8 := \""     + fmap.at("g_prs_cons_utf8")     + "\";\n"
        "    g_vbs_cons_utf8 := \""     + fmap.at("g_vbs_cons_utf8")     + "\";\n"
        "    g_vbe_cons_utf8 := \""     + fmap.at("g_vbe_cons_utf8")     + "\";\n"
        "    g_uvf_cons_utf8 := \""     + fmap.at("g_uvf_cons_utf8")     + "\";\n"
        "    lex_cons_utf8 := \""       + fmap.at("lex_cons_utf8")       + "\";\n"
        "    g_voc_lex_cons_utf8 := \"" + fmap.at("g_voc_lex_cons_utf8") + "\";\n"
        "    g_word_nocant := \""       + fmap.at("g_word_nocant")       + "\";\n"
        "    g_word_nocant_utf8 := \""  + fmap.at("g_word_nocant_utf8")  + "\";\n"
        "    g_word_cons_utf8 := \""    + fmap.at("g_word_cons_utf8")    + "\";\n";
}
