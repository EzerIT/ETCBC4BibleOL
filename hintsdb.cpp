/* Copyright © 2022 Claus Tøndering.
 * Released under an MIT License.
 */
 
#include <emdros/emdfdb.h>
#include <emdros/emdros_environment.h>
#include <emdros/mql_sheaf.h>
#include <emdros/emdf_value.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "rapidcsv/src/rapidcsv.h"

#include "emdros_iterators.hpp"
#include "util.hpp"

using namespace std;

enum class cols {
    original_order,book,chapter,verse,lex,verb_classes,verb_classes_correction,
    g_word_nocant,not_usable_for_exercises_because_kq,g_word_nocant_utf8,
    ps1,nu1,gn1,vt1,vs1,suffix_person1,suffix_number1,suffix_gender1,
    ps2,nu2,gn2,vt2,vs2,suffix_person2,suffix_number2,suffix_gender2,
    ps3,nu3,gn3,vt3,vs3,suffix_person3,suffix_number3,suffix_gender3,
    ps4,nu4,gn4,vt4,vs4,suffix_person4,suffix_number4,suffix_gender4
};

string at(const vector<string>& r, int c)
{
    if (c >= r.size())
        return "unknown";
    else
        return r.at(c);
}

string at(const vector<string>& r, cols c)
{
    return at(r,(int)c);
}

int count_var(const vector<string>& r)
{
    int count = 0;

    if (!at(r,cols::ps1).empty()) ++count;
    if (!at(r,cols::ps2).empty()) ++count;
    if (!at(r,cols::ps3).empty()) ++count;
    if (!at(r,cols::ps4).empty()) ++count;

    return count;
}


class selector {
  public:
    static string diff2(const vector<string>&r) {
        for (int i=0; i<feat_diff2.size(); ++i) {
            cols col1 = feat_diff1[i];
            cols col2 = feat_diff2[i];

            if (alldiff(at(r,col1), at(r,col2)))
                return feat_diff2string.at(i) + "=" + at(r,col1);
        }
        for (auto rc : r)
            cerr << rc << " ";
        cerr << "INDETERMINATE 1\n";
        return "INDETERMINATE 1";
    }

    static string diff3(const vector<string>&r) {
        for (int i=0; i<feat_diff1.size(); ++i) {
            cols col1 = feat_diff1[i];
            cols col2 = feat_diff2[i];
            cols col3 = feat_diff3[i];
            
            if (alldiff(at(r,col1), at(r,col2), at(r,col3)))
                return feat_diff2string.at(i) + "=" + at(r,col1);
        }
        for (auto rc : r)
            cerr << rc << " ";
        cerr << "INDETERMINATE 2\n";
        return "INDETERMINATE 2";
    }

    static string diff4(const vector<string>&r) {
        for (int i=0; i<feat_diff1.size(); ++i) {
            cols col1 = feat_diff1[i];
            cols col2 = feat_diff2[i];
            cols col3 = feat_diff3[i];
            cols col4 = feat_diff4[i];
            
            if (alldiff(at(r,col1), at(r,col2), at(r,col3), at(r,col4)))
                return feat_diff2string.at(i) + "=" + at(r,col1);
        }

        // Indeterminate based on one feature, let's try two features
        for (int i=0; i<feat_diff1.size(); ++i) {
            for (int j=i+1; j<feat_diff1.size(); ++j) {
            
                string s1a = at(r, feat_diff1[i]);
                string s2a = at(r, feat_diff2[i]);
                string s3a = at(r, feat_diff3[i]);
                string s4a = at(r, feat_diff4[i]);
                string s1b = at(r, feat_diff1[j]);
                string s2b = at(r, feat_diff2[j]);
                string s3b = at(r, feat_diff3[j]);
                string s4b = at(r, feat_diff4[j]);

                if (s1a=="unknown" || s1b=="unknown" ||
                    s2a=="unknown" || s2b=="unknown" ||
                    s3a=="unknown" || s3b=="unknown" ||
                    s4a=="unknown" || s4b=="unknown")
                    continue;
                
                string s1 = s1a + "." + s1b;
                string s2 = s2a + "." + s2b;
                string s3 = s3a + "." + s3b;
                string s4 = s4a + "." + s4b;

                if (alldiff(s1,s2,s3,s4))
                    return feat_diff2string.at(i) + "=" + s1a + ","
                        + feat_diff2string.at(j) + "=" + s1b;
            }
        }

        // Indeterminate based on two features, let's allow "unknown" values
        for (int i=0; i<feat_diff1.size(); ++i) {
            for (int j=i+1; j<feat_diff1.size(); ++j) {
            
                string s1a = at(r, feat_diff1[i]);
                string s2a = at(r, feat_diff2[i]);
                string s3a = at(r, feat_diff3[i]);
                string s4a = at(r, feat_diff4[i]);
                string s1b = at(r, feat_diff1[j]);
                string s2b = at(r, feat_diff2[j]);
                string s3b = at(r, feat_diff3[j]);
                string s4b = at(r, feat_diff4[j]);

                string s1 = s1a + "." + s1b;
                string s2 = s2a + "." + s2b;
                string s3 = s3a + "." + s3b;
                string s4 = s4a + "." + s4b;

                if (alldiff(s1,s2,s3,s4))
                    return feat_diff2string.at(i) + "=" + s1a + ","
                        + feat_diff2string.at(j) + "=" + s1b;
            }
        }

        for (auto rc : r)
            cerr << rc << " ";
        cerr << "INDETERMINATE 3\n";
        return "INDETERMINATE 3";
    }

  private:
    // This identifies the feature differentiators by order of relevance.
    inline static array feat_diff1{cols::ps1, cols::gn1, cols::nu1, cols::vt1, cols::vs1, cols::suffix_person1, cols::suffix_number1, cols::suffix_gender1};
    inline static array feat_diff2{cols::ps2, cols::gn2, cols::nu2, cols::vt2, cols::vs2, cols::suffix_person2, cols::suffix_number2, cols::suffix_gender2};
    inline static array feat_diff3{cols::ps3, cols::gn3, cols::nu3, cols::vt3, cols::vs3, cols::suffix_person3, cols::suffix_number3, cols::suffix_gender3};
    inline static array feat_diff4{cols::ps4, cols::gn4, cols::nu4, cols::vt4, cols::vs4, cols::suffix_person4, cols::suffix_number4, cols::suffix_gender4};

    // Translates int(feat_diff) to string
    inline static array feat_diff2string{"ps"s, "nu"s, "gn"s, "vt"s, "vs"s, "suffix_person"s, "suffix_number"s, "suffix_gender"s};

    static bool alldiff(const string& s1, const string& s2) {
        return s1!=s2 && s1!="unknown" && s2!="unknown";
    }

    static bool alldiff(const string& s1, const string& s2, const string& s3) {
        return s1!=s2 && s1!=s3 && s2!=s3
            && s1!="unknown" && s2!="unknown" && s3!="unknown";
    }

    static bool alldiff(const string& s1, const string& s2, const string& s3, const string& s4) {
        return s1!=s2 && s1!=s3 && s1!=s4 && s2!=s3 && s2!=s4 && s3!=s4
            && s1!="unknown" && s2!="unknown" && s3!="unknown" && s4!="uknown";
    }
};
 

string fix_g_word_nocant(string text_nocant)
{
    replace_string_in_place(text_nocant, "25", "");  // Removing meteg
    replace_string_in_place(text_nocant, "45", "");  // Removing meteg

    return text_nocant;
}


int main(int argc, char **argv)
{
    if (argc!=3) {
        cerr << "usage: hintsdb <emdrosfile> <sqlfile>" << endl;
        return 1;
    }

    ofstream sqlfile{argv[2]};
    sqlfile << "CREATE TABLE hints (self integer primary key, hint text);\n";
    sqlfile << "BEGIN TRANSACTION;\n";

    
    EmdrosEnv EE{kOKConsole,
            kCSUTF8,
            "localhost",
            "",
            "",
            argv[1],
            kSQLite3};

    bool bResult{false};

    for (const string& lang : { "Hebrew", "Aramaic" }) {
        rapidcsv::Document csv;
        int csv_row = 0;
        
        string csvfile{lang=="Hebrew"
            ? "BibleOL_verbal-ambiguity-project_v1.43b-heb.csv"
            : "BibleOL_verbal-ambiguity-project_v1.43b-aram.csv"};

        try {
            csv.Load(csvfile);
        }
        catch (const ios_base::failure& e) {
            cerr << e.what() << "\n";
            cerr << "Cannot open " << csvfile << "\n";
            return 1;
        }

        string mql_request{"SELECT ALL OBJECTS WHERE [word sp=verb AND language=" + lang + " GET self,g_word_nocant,"
            "ps,nu,gn,vt,vs,suffix_person,suffix_number,suffix_gender] GO"};
    
        if (!EE.executeString(mql_request, bResult, false, true))
            return 1;

        if (!EE.isSheaf()) {
            cerr << "ERROR: Result is not sheaf\n";
            return 1;
        }

        for (StrawOk str : SheafOk{EE.getSheaf()}) {
            long self = 0;
            string g_word_nocant;
            string features[8]; //ps,nu,gn,vt,vs,suffix_person,suffix_number,suffix_gender
        
            for (const MatchedObject mo : str) {
                self = mo.getFeatureAsLong(0);
                g_word_nocant = fix_g_word_nocant(mo.getFeatureAsString(1));
                for (int i=0; i<8; ++i) {
                    features[i] = mo.getFeatureAsString(2+i);
                    if (i>=5 && features[i]=="absent")
                        features[i]="unknown";
                }
            }

            vector<string> row = csv.GetRow<string>(csv_row++);
        
            if (g_word_nocant != at(row,cols::g_word_nocant)) {
                cerr << "Inconsistency between database and spreadsheet at original order " << at(row,cols::original_order) << '\n'
                     << "Spreadsheet has " << at(row,cols::g_word_nocant)
                     << " Emdros has " << g_word_nocant << '\n';
                return 1;
            }

            for (int i=0; i<8; ++i) {
                if (features[i] != at(row,int(cols::ps1) + i)) {
                    cerr << "Inconsistency between database and spreadsheet at original order " << at(row,cols::original_order) << " word is " << g_word_nocant << '\n'
                         << "Spreadsheet has feature " << i << "=" << at(row,int(cols::ps1)+i)
                         << " Emdros has " << features[i] << '\n';
//                return 1;
                }
            }


            switch (count_var(row)) {
              case 4:
                    sqlfile << "INSERT INTO hints VALUES(" << self << ",'" << selector::diff4(row) << "');\n";
                    break;
              case 3:
                    sqlfile << "INSERT INTO hints VALUES(" << self << ",'" << selector::diff3(row) << "');\n";
                    break;
              case 2:
                    sqlfile << "INSERT INTO hints VALUES(" << self << ",'" << selector::diff2(row) << "');\n";
                    break;
            }
        }
    }
    
    sqlfile << "COMMIT;\n";
}
