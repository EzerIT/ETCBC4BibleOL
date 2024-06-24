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
#include <algorithm>
#include "rapidcsv/src/rapidcsv.h"

#include "emdros_iterators.hpp"
#include "util.hpp"

using namespace std;

// Unified columns
enum class cols {
    original_order,book,chapter,verse,lex,verb_classes,verb_classes_correction,
    g_word_nocant,g_word_noaccent1,not_usable_for_exercises_because_kq,g_word_nocant_utf8,
    ps1,nu1,gn1,vt1,vs1,suffix_person1,suffix_number1,suffix_gender1,st1,
    ps2,nu2,gn2,vt2,vs2,suffix_person2,suffix_number2,suffix_gender2,st2,
    ps3,nu3,gn3,vt3,vs3,suffix_person3,suffix_number3,suffix_gender3,st3,
    ps4,nu4,gn4,vt4,vs4,suffix_person4,suffix_number4,suffix_gender4,st4, 
    ps5,nu5,gn5,vt5,vs5,suffix_person5,suffix_number5,suffix_gender5,st5, 
    ps6,nu6,gn6,vt6,vs6,suffix_person6,suffix_number6,suffix_gender6,st6, 
    ps7,nu7,gn7,vt7,vs7,suffix_person7,suffix_number7,suffix_gender7,st7, 
    ps8,nu8,gn8,vt8,vs8,suffix_person8,suffix_number8,suffix_gender8,st8,
    COLMAX
};

// Columns in Aramaic file
enum class cols_aram {
    original_order,book,chapter,verse,lex,verb_classes,verb_classes_correction,
    g_word_noaccent1,not_usable_for_exercises_because_kq,g_word_nocant_utf8,
    ps1,nu1,gn1,vt1,vs1,suffix_person1,suffix_number1,suffix_gender1,st1,
    ps2,nu2,gn2,vt2,vs2,suffix_person2,suffix_number2,suffix_gender2,st2,
    ps3,nu3,gn3,vt3,vs3,suffix_person3,suffix_number3,suffix_gender3,st3,
    ps4,nu4,gn4,vt4,vs4,suffix_person4,suffix_number4,suffix_gender4,st4, 
    COLMAX
};

// Columns in Hebrew file
enum class cols_heb {
    original_order,book,chapter,verse,lex,verb_classes,
    g_word_nocant,g_word_nocant_utf8,
    ps1,nu1,gn1,vt1,vs1,suffix_person1,suffix_number1,suffix_gender1,st1,
    ps2,nu2,gn2,vt2,vs2,suffix_person2,suffix_number2,suffix_gender2,st2,
    ps3,nu3,gn3,vt3,vs3,suffix_person3,suffix_number3,suffix_gender3,st3,
    ps4,nu4,gn4,vt4,vs4,suffix_person4,suffix_number4,suffix_gender4,st4, 
    ps5,nu5,gn5,vt5,vs5,suffix_person5,suffix_number5,suffix_gender5,st5, 
    ps6,nu6,gn6,vt6,vs6,suffix_person6,suffix_number6,suffix_gender6,st6, 
    ps7,nu7,gn7,vt7,vs7,suffix_person7,suffix_number7,suffix_gender7,st7, 
    ps8,nu8,gn8,vt8,vs8,suffix_person8,suffix_number8,suffix_gender8,st8, 
    COLMAX
};
    
vector<string> unify(bool is_hebrew, const vector<string>& columns)
{
    vector<string> u((int)cols::COLMAX);
    
    if (is_hebrew) {
        for (int i=0; i<min((vector<string>::size_type)cols_heb::COLMAX,columns.size()); ++i) {
            cols destix;
            switch ((cols_heb)i) {
              case cols_heb::original_order:          destix = cols::original_order;          break;
              case cols_heb::book:                    destix = cols::book;                    break;
              case cols_heb::chapter:                 destix = cols::chapter;                 break;
              case cols_heb::verse:                   destix = cols::verse;                   break;
              case cols_heb::lex:                     destix = cols::lex;                     break;
              case cols_heb::verb_classes:            destix = cols::verb_classes;            break;
              case cols_heb::g_word_nocant:           destix = cols::g_word_nocant;           break;
              case cols_heb::g_word_nocant_utf8:      destix = cols::g_word_nocant_utf8;      break;
              case cols_heb::ps1:                     destix = cols::ps1;                     break;
              case cols_heb::nu1:                     destix = cols::nu1;                     break;
              case cols_heb::gn1:                     destix = cols::gn1;                     break;
              case cols_heb::vt1:                     destix = cols::vt1;                     break;
              case cols_heb::vs1:                     destix = cols::vs1;                     break;
              case cols_heb::suffix_person1:          destix = cols::suffix_person1;          break;
              case cols_heb::suffix_number1:          destix = cols::suffix_number1;          break;
              case cols_heb::suffix_gender1:          destix = cols::suffix_gender1;          break;
              case cols_heb::st1:                     destix = cols::st1;                     break;
              case cols_heb::ps2:                     destix = cols::ps2;                     break;
              case cols_heb::nu2:                     destix = cols::nu2;                     break;
              case cols_heb::gn2:                     destix = cols::gn2;                     break;
              case cols_heb::vt2:                     destix = cols::vt2;                     break;
              case cols_heb::vs2:                     destix = cols::vs2;                     break;
              case cols_heb::suffix_person2:          destix = cols::suffix_person2;          break;
              case cols_heb::suffix_number2:          destix = cols::suffix_number2;          break;
              case cols_heb::suffix_gender2:          destix = cols::suffix_gender2;          break;
              case cols_heb::st2:                     destix = cols::st2;                     break;
              case cols_heb::ps3:                     destix = cols::ps3;                     break;
              case cols_heb::nu3:                     destix = cols::nu3;                     break;
              case cols_heb::gn3:                     destix = cols::gn3;                     break;
              case cols_heb::vt3:                     destix = cols::vt3;                     break;
              case cols_heb::vs3:                     destix = cols::vs3;                     break;
              case cols_heb::suffix_person3:          destix = cols::suffix_person3;          break;
              case cols_heb::suffix_number3:          destix = cols::suffix_number3;          break;
              case cols_heb::suffix_gender3:          destix = cols::suffix_gender3;          break;
              case cols_heb::st3:                     destix = cols::st3;                     break;
              case cols_heb::ps4:                     destix = cols::ps4;                     break;
              case cols_heb::nu4:                     destix = cols::nu4;                     break;
              case cols_heb::gn4:                     destix = cols::gn4;                     break;
              case cols_heb::vt4:                     destix = cols::vt4;                     break;
              case cols_heb::vs4:                     destix = cols::vs4;                     break;
              case cols_heb::suffix_person4:          destix = cols::suffix_person4;          break;
              case cols_heb::suffix_number4:          destix = cols::suffix_number4;          break;
              case cols_heb::suffix_gender4:          destix = cols::suffix_gender4;          break;
              case cols_heb::st4:                     destix = cols::st4;                     break;
              case cols_heb::ps5:                     destix = cols::ps5;                     break;
              case cols_heb::nu5:                     destix = cols::nu5;                     break;
              case cols_heb::gn5:                     destix = cols::gn5;                     break;
              case cols_heb::vt5:                     destix = cols::vt5;                     break;
              case cols_heb::vs5:                     destix = cols::vs5;                     break;
              case cols_heb::suffix_person5:          destix = cols::suffix_person5;          break;
              case cols_heb::suffix_number5:          destix = cols::suffix_number5;          break;
              case cols_heb::suffix_gender5:          destix = cols::suffix_gender5;          break;
              case cols_heb::st5:                     destix = cols::st5;                     break;
              case cols_heb::ps6:                     destix = cols::ps6;                     break;
              case cols_heb::nu6:                     destix = cols::nu6;                     break;
              case cols_heb::gn6:                     destix = cols::gn6;                     break;
              case cols_heb::vt6:                     destix = cols::vt6;                     break;
              case cols_heb::vs6:                     destix = cols::vs6;                     break;
              case cols_heb::suffix_person6:          destix = cols::suffix_person6;          break;
              case cols_heb::suffix_number6:          destix = cols::suffix_number6;          break;
              case cols_heb::suffix_gender6:          destix = cols::suffix_gender6;          break;
              case cols_heb::st6:                     destix = cols::st6;                     break;
              case cols_heb::ps7:                     destix = cols::ps7;                     break;
              case cols_heb::nu7:                     destix = cols::nu7;                     break;
              case cols_heb::gn7:                     destix = cols::gn7;                     break;
              case cols_heb::vt7:                     destix = cols::vt7;                     break;
              case cols_heb::vs7:                     destix = cols::vs7;                     break;
              case cols_heb::suffix_person7:          destix = cols::suffix_person7;          break;
              case cols_heb::suffix_number7:          destix = cols::suffix_number7;          break;
              case cols_heb::suffix_gender7:          destix = cols::suffix_gender7;          break;
              case cols_heb::st7:                     destix = cols::st7;                     break;
              case cols_heb::ps8:                     destix = cols::ps8;                     break;
              case cols_heb::nu8:                     destix = cols::nu8;                     break;
              case cols_heb::gn8:                     destix = cols::gn8;                     break;
              case cols_heb::vt8:                     destix = cols::vt8;                     break;
              case cols_heb::vs8:                     destix = cols::vs8;                     break;
              case cols_heb::suffix_person8:          destix = cols::suffix_person8;          break;
              case cols_heb::suffix_number8:          destix = cols::suffix_number8;          break;
              case cols_heb::suffix_gender8:          destix = cols::suffix_gender8;          break;
              case cols_heb::st8:                     destix = cols::st8;                     break;
            }
            u[(int)destix] = columns[i];
        }
    }
    else {
        for (int i=0; i<min((vector<string>::size_type)cols_aram::COLMAX,columns.size()); ++i) {
            cols destix;
            switch ((cols_aram)i) {
              case cols_aram::original_order:                      destix = cols::original_order;                      break;
              case cols_aram::book:                                destix = cols::book;                                break;
              case cols_aram::chapter:                             destix = cols::chapter;                             break;
              case cols_aram::verse:                               destix = cols::verse;                               break;
              case cols_aram::lex:                                 destix = cols::lex;                                 break;
              case cols_aram::verb_classes:                        destix = cols::verb_classes;                        break;
              case cols_aram::verb_classes_correction:             destix = cols::verb_classes_correction;             break;
              case cols_aram::g_word_noaccent1:                    destix = cols::g_word_noaccent1;                    break;
              case cols_aram::not_usable_for_exercises_because_kq: destix = cols::not_usable_for_exercises_because_kq; break;
              case cols_aram::g_word_nocant_utf8:                  destix = cols::g_word_nocant_utf8;                  break;
              case cols_aram::ps1:                                 destix = cols::ps1;                                 break;
              case cols_aram::nu1:                                 destix = cols::nu1;                                 break;
              case cols_aram::gn1:                                 destix = cols::gn1;                                 break;
              case cols_aram::vt1:                                 destix = cols::vt1;                                 break;
              case cols_aram::vs1:                                 destix = cols::vs1;                                 break;
              case cols_aram::suffix_person1:                      destix = cols::suffix_person1;                      break;
              case cols_aram::suffix_number1:                      destix = cols::suffix_number1;                      break;
              case cols_aram::suffix_gender1:                      destix = cols::suffix_gender1;                      break;
              case cols_aram::st1:                                 destix = cols::st1;                                 break;
              case cols_aram::ps2:                                 destix = cols::ps2;                                 break;
              case cols_aram::nu2:                                 destix = cols::nu2;                                 break;
              case cols_aram::gn2:                                 destix = cols::gn2;                                 break;
              case cols_aram::vt2:                                 destix = cols::vt2;                                 break;
              case cols_aram::vs2:                                 destix = cols::vs2;                                 break;
              case cols_aram::suffix_person2:                      destix = cols::suffix_person2;                      break;
              case cols_aram::suffix_number2:                      destix = cols::suffix_number2;                      break;
              case cols_aram::suffix_gender2:                      destix = cols::suffix_gender2;                      break;
              case cols_aram::st2:                                 destix = cols::st2;                                 break;
              case cols_aram::ps3:                                 destix = cols::ps3;                                 break;
              case cols_aram::nu3:                                 destix = cols::nu3;                                 break;
              case cols_aram::gn3:                                 destix = cols::gn3;                                 break;
              case cols_aram::vt3:                                 destix = cols::vt3;                                 break;
              case cols_aram::vs3:                                 destix = cols::vs3;                                 break;
              case cols_aram::suffix_person3:                      destix = cols::suffix_person3;                      break;
              case cols_aram::suffix_number3:                      destix = cols::suffix_number3;                      break;
              case cols_aram::suffix_gender3:                      destix = cols::suffix_gender3;                      break;
              case cols_aram::st3:                                 destix = cols::st3;                                 break;
              case cols_aram::ps4:                                 destix = cols::ps4;                                 break;
              case cols_aram::nu4:                                 destix = cols::nu4;                                 break;
              case cols_aram::gn4:                                 destix = cols::gn4;                                 break;
              case cols_aram::vt4:                                 destix = cols::vt4;                                 break;
              case cols_aram::vs4:                                 destix = cols::vs4;                                 break;
              case cols_aram::suffix_person4:                      destix = cols::suffix_person4;                      break;
              case cols_aram::suffix_number4:                      destix = cols::suffix_number4;                      break;
              case cols_aram::suffix_gender4:                      destix = cols::suffix_gender4;                      break;
              case cols_aram::st4:                                 destix = cols::st4;                                 break;
            }
            u[(int)destix] = columns[i];
        }
    }

    return u;
}


    
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
    if (!at(r,cols::ps5).empty()) ++count;
    if (!at(r,cols::ps6).empty()) ++count;
    if (!at(r,cols::ps7).empty()) ++count;
    if (!at(r,cols::ps8).empty()) ++count;

    return count;
}


class selector {
  public:
    static string diff2(const vector<string>&r) {
        for (int i=0; i<feat_diff2.size(); ++i) {
            cols col1 = feat_diff1[i];
            cols col2 = feat_diff2[i];

            if (alldiff(at(r,col1), at(r,col2)))
                if (twovalues.contains(i))
                    return feat_diff2string.at(i) + "=" + at(r,col1);
                else
                    return feat_diff2string.at(i) + "≠" + at(r,col2);
        }

        // We need to allow for "unknown" values
        for (int i=0; i<feat_diff2.size(); ++i) {
            cols col1 = feat_diff1[i];
            cols col2 = feat_diff2[i];

            if (alldiff1u(at(r,col1), at(r,col2)))
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
                if (twovalues.contains(i))
                    return feat_diff2string.at(i) + "=" + at(r,col1);
                else
                    return feat_diff2string.at(i) + "≠" + at(r,col2) + "," + feat_diff2string.at(i) + "≠" + at(r,col3);
        }

        // If we reach this point, we look for a value that distinguishes just the primary (correct) option.
        // As a matter of fact, we could use this mechanism everywhere, but by now the hints found
        // by the "alldiff" methods are well established, so we'll leave them in place.

        for (int i=0; i<feat_diff1.size(); ++i) {
            cols col1 = feat_diff1[i];
            cols col2 = feat_diff2[i];
            cols col3 = feat_diff3[i];
        
            if (alldiff23u(at(r,col1), at(r,col2), at(r,col3)))
                if (threevalues.contains(i) || at(r,col2)!=at(r,col3))
                    return feat_diff2string.at(i) + "=" + at(r,col1);
                else
                    return feat_diff2string.at(i) + "≠" + at(r,col2);
        }

        // Indeterminate based on two features, let's allow "unknown" values
        for (int i=0; i<feat_diff1.size(); ++i) {
            for (int j=i+1; j<feat_diff1.size(); ++j) {
            
                string s1a = at(r, feat_diff1[i]);
                string s2a = at(r, feat_diff2[i]);
                string s3a = at(r, feat_diff3[i]);
                string s1b = at(r, feat_diff1[j]);
                string s2b = at(r, feat_diff2[j]);
                string s3b = at(r, feat_diff3[j]);

                string s1 = s1a + "." + s1b;
                string s2 = s2a + "." + s2b;
                string s3 = s3a + "." + s3b;

                if (alldiff(s1,s2,s3))
                    return feat_diff2string.at(i) + "=" + s1a + ","
                        + feat_diff2string.at(j) + "=" + s1b;
            }
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

    // Note: diff8() is programmed very ad-hoc. Future information may require a more sophisticated mechanism
    static string diff8(const vector<string>&r) {
        // We assume that the appropriate hint string will be
        // ps≠p3,vs≠hif,suffix_person≠p1

        if (at(r,cols::ps1)!="p3" && at(r,cols::vs1)!="hif" && at(r,cols::suffix_person1)!="p1" &&
            !(at(r,cols::ps2)!="p3" && at(r,cols::vs2)!="hif" && at(r,cols::suffix_person2)!="p1") &&
            !(at(r,cols::ps3)!="p3" && at(r,cols::vs3)!="hif" && at(r,cols::suffix_person3)!="p1") &&
            !(at(r,cols::ps4)!="p3" && at(r,cols::vs4)!="hif" && at(r,cols::suffix_person4)!="p1") &&
            !(at(r,cols::ps5)!="p3" && at(r,cols::vs5)!="hif" && at(r,cols::suffix_person5)!="p1") &&
            !(at(r,cols::ps6)!="p3" && at(r,cols::vs6)!="hif" && at(r,cols::suffix_person6)!="p1") &&
            !(at(r,cols::ps7)!="p3" && at(r,cols::vs7)!="hif" && at(r,cols::suffix_person7)!="p1") &&
            !(at(r,cols::ps8)!="p3" && at(r,cols::vs8)!="hif" && at(r,cols::suffix_person8)!="p1"))
            return "ps≠p3,vs≠hif,suffix_person≠p1";

        for (auto rc : r)
            cerr << rc << " ";
        cerr << "INDETERMINATE 4\n";
        return "INDETERMINATE 4";
    }

  private:
    // This identifies the feature differentiators by order of relevance.
    inline static array feat_diff1{cols::ps1, cols::gn1, cols::nu1, cols::vt1, cols::vs1, cols::suffix_person1, cols::suffix_number1, cols::suffix_gender1, cols::st1};
    inline static array feat_diff2{cols::ps2, cols::gn2, cols::nu2, cols::vt2, cols::vs2, cols::suffix_person2, cols::suffix_number2, cols::suffix_gender2, cols::st2};
    inline static array feat_diff3{cols::ps3, cols::gn3, cols::nu3, cols::vt3, cols::vs3, cols::suffix_person3, cols::suffix_number3, cols::suffix_gender3, cols::st3};
    inline static array feat_diff4{cols::ps4, cols::gn4, cols::nu4, cols::vt4, cols::vs4, cols::suffix_person4, cols::suffix_number4, cols::suffix_gender4, cols::st4};

    // This identifies the index into feat_diffX for features with 2 or 3 main values:
    inline static set<int> twovalues{1,7}; // gender, suffix_gender
    inline static set<int> threevalues{0,2,5,6,8}; // person, number, suffix_person, suffix_number, state
    
    // Translates int(feat_diff) to string
    inline static array feat_diff2string{"ps"s, "gn"s, "nu"s, "vt"s, "vs"s, "suffix_person"s, "suffix_number"s, "suffix_gender"s, "st"s};

    // Test if s1!=s2 and neither is "unknown"
    static bool alldiff(const string& s1, const string& s2) {
        return s1!=s2 && s1!="unknown" && s2!="unknown";
    }

    // Like alldiff(s1,s2), but s1 may be "unknown"
    static bool alldiff1u(const string& s1, const string& s2) {
        return s1!=s2 && s2!="unknown";
    }

    // Test if s1, s2, and s3 are all different and neither is "unknown"
    static bool alldiff(const string& s1, const string& s2, const string& s3) {
        return s1!=s2 && s1!=s3 && s2!=s3
            && s1!="unknown" && s2!="unknown" && s3!="unknown";
    }

    // Like alldiff(s1,s2,s3), but s2 or s3 may be "unknown"
    static bool alldiff23u(const string& s1, const string& s2, const string& s3) {
        return s1!=s2 && s1!=s3
            && s1!="unknown";
    }

    // Test if s1, s2, s3 and s4 are all different and neither is "unknown"
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
        cerr << "---------------------------------" << lang << "-----------------------------\n";
        rapidcsv::Document csv;
        int csv_row = 0;
        
        string csvfile{lang=="Hebrew"
            ? "HEBREW_BibleOL_verbal-ambiguity-project_v1.57-heb.csv"
            : "BibleOL_verbal-ambiguity-project_v1.48-aram.csv"};

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

            vector<string> row = unify(lang=="Hebrew",csv.GetRow<string>(csv_row++));

            // For Aramaic column cols:g_word_nocant is empty, so we just check
            // this column in the Hebrew case
            if (lang=="Hebrew" && g_word_nocant != at(row,cols::g_word_nocant)) {
                cerr << "Warning: Inconsistency between database and spreadsheet at original order " << at(row,cols::original_order) << '\n'
                     << "Spreadsheet has " << at(row,cols::g_word_nocant)
                     << " Emdros has " << g_word_nocant << '\n';
            }

            for (int i=0; i<8; ++i) {
                if (features[i] != at(row,int(cols::ps1) + i)) {
                    cerr << "Warning: Inconsistency between database and spreadsheet at original order " << at(row,cols::original_order) << " word is " << g_word_nocant << '\n'
                         << "Spreadsheet has feature " << i << "=" << at(row,int(cols::ps1)+i)
                         << " Emdros has " << features[i] << '\n';
                    if (i==3 && features[i]=="juss" && at(row,int(cols::ps1)+i)=="impf") {
                        row[int(cols::ps1)+i] = features[i];
                        cerr << "Corrected to " << at(row,int(cols::ps1)+i) << "\n";
                    }
                }
            }


            switch (count_var(row)) {
              case 8:
                    sqlfile << "INSERT INTO hints VALUES(" << self << ",'" << selector::diff8(row) << "');\n";
                    break;
              case 4:
                    sqlfile << "INSERT INTO hints VALUES(" << self << ",'" << selector::diff4(row) << "');\n";
                    break;
              case 3:
                    sqlfile << "INSERT INTO hints VALUES(" << self << ",'" << selector::diff3(row) << "');\n";
                    break;
              case 2:
                    sqlfile << "INSERT INTO hints VALUES(" << self << ",'" << selector::diff2(row) << "');\n";
                    break;
              case 1:
                    break;
              default:
                    cerr << "A line with " << count_var(row) << " alternatives found at original order "
                         << at(row,cols::original_order) << ". This number of alternatives is not supported.\n";
                    break;
            }
        }
    }
    
    sqlfile << "COMMIT;\n";
}
