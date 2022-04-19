/* Copyright © 2022 Claus Tøndering.
 * Released under an MIT License.
 */

/****************************************************************************************************
 * This files takes BibleOL_dictionary_Hebrew-Spanish_vX.X.csv file as input and generates
 * a heb_es.csv file as output.
 ****************************************************************************************************/

#include <vector>
#include <map>
#include <iostream>
#include "rapidcsv/src/rapidcsv.h"

using namespace std;

// Headers in input file:
//      0 section
//      1 Frequency rank
//      2 Occurrences
//      3 absolut alphabetic order
//      4 lex
//      5 Vocalized lexeme
//      6 verbal class 1st cons
//      7 verbal class 2nd cons
//      8 verbal class 3rd cons
//      9 verbal class other
//     10 verbal class list with formula
//     11 verbal class list witout formula
//     12 Part of speech
//     13 Verbal stems
//     14 Current EN glosses
//     15 Vocalized lexeme as in Mitchel (print edition)
//     16 Vocalized lexeme as in Mitchel (Logos version)
//     17 Mitchel Section
//     18 Rational for change
//     19 Mitchel non-verb gloss ES
//     20 New glosses_with_prioritized displayed meaning of verbs_without_excel_formula_ES
//     21 New glosses (verbs:prioritized displayed meaning of verbs) with excel formula ES
//     22 meaning fixed
//     23 Qal=1 ES
//     24 Nifal=2 ES
//     25 Piel=3 ES
//     26 Pual=4 ES
//     27 Hitpael=5 ES
//     28 Hifil=6 ES
//     29 Hofal=7 ES
//     30 Hishtafal=8 ES
//     31 Passive qal=9 ES
//     32 Etpaal=1 ES
//     33 Nitpael=11 ES
//     34 Hotpaal=12 ES
//     35 Tifal=13 ES
//     36 Hitpoal=14 ES
//     37 Poal=15 ES
//     38 Poel=16 ES
//--------------------------------
// Headers in output file:
//      0 Occurrences
//      1 lex
//      2 Lexeme
//      3 Transliterated
//      4 None
//      5 Qal
//      6 Nifal
//      7 Piel
//      8 Pual
//      9 Hitpael
//     10 Hifil
//     11 Hofal
//     12 Hishtafal
//     13 Passive Qal
//     14 Etpaal
//     15 Nitpael
//     16 Hotpaal
//     17 Tifal
//     18 Hitpoal
//     19 Poal
//     20 Poel


constexpr int MAX_OUT_FIELD = 21;

// For verbs:
map<int,int> output2input = { {  1,  4 },   // lex
                              {  5, 23 },   // Qal        
                              {  6, 24 },   // Nifal      
                              {  7, 25 },   // Piel       
                              {  8, 26 },   // Pual       
                              {  9, 27 },   // Hitpael    
                              { 10, 28 },   // Hifil      
                              { 11, 29 },   // Hofal      
                              { 12, 30 },   // Hishtafal  
                              { 13, 31 },   // Passive Qal
                              { 14, 32 },   // Etpaal     
                              { 15, 33 },   // Nitpael    
                              { 16, 34 },   // Hotpaal    
                              { 17, 35 },   // Tifal      
                              { 18, 36 },   // Hitpoal    
                              { 19, 37 },   // Poal       
                              { 20, 38 } }; // Poel       


int main()
{
    string inputfilename{"BibleOL_dictionary_Hebrew-Spanish_v1.2.csv"};
    string outputfilename{"heb_es.csv"};
    
    try {
        rapidcsv::Document csvfile{inputfilename};

        // In output file we always set quotation marks (indicated by last of the SeparatorParams)
        rapidcsv::Document outfile{
            "",
            rapidcsv::LabelParams{}, 
            rapidcsv::SeparatorParams{',',false, rapidcsv::sPlatformHasCR, false, false}};

        // Generate header line in output file
        int colno = 0;
        for (const string& s :
                 {"Occurrences","lex","Lexeme","Transliterated","None","Qal","Nifal","Piel","Pual","Hitpael","Hifil",
                  "Hofal","Hishtafal","Passive Qal","Etpaal","Nitpael","Hotpaal","Tifal","Hitpoal","Poal","Poel"})
            outfile.SetColumnName(colno++,'"' + s + '"');

        // Convert each line
        for (size_t rix=0; rix<csvfile.GetRowCount(); ++rix) {
            vector<string> row = csvfile.GetRow<string>(rix);

            if (row[12]=="verb") {
                for (int cix=0; cix<MAX_OUT_FIELD; ++cix)
                    if (output2input.count(cix)>0) {
                        string s = row[output2input[cix]];
                        outfile.SetCell<string>(cix, rix, s.empty() ? "" : '"' + row[output2input[cix]] + '"');
                    }
            }
            else { // Not a verb
                outfile.SetCell<string>(1, rix, '"' + row[4] + '"');
                outfile.SetCell<string>(4, rix, '"' + row[20] + '"');
            }
        }
        
        try {
            outfile.Save(outputfilename);
        }
        catch (const ios_base::failure& e) {
            cerr << e.what() << "\n";
            cerr << "Cannot write " << outputfilename << "\n";
            return 1;
        }
    }
    catch (const ios_base::failure& e) {
        cerr << e.what() << "\n";
        cerr << "Cannot open " << inputfilename << "\n";
        return 1;
    }

}
 
