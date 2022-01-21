/* Copyright © 2022 Claus Tøndering.
 * Released under an MIT License.
 */

/****************************************************************************************************
 * This files takes a ETCBC4-frequencyX.XX_progression-aram.csv file as input and generates
 * an aram_en.csv file as output.
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
//     15 x
//     16 x
//     17 x
//     18 x
//     19 New glosses, non-verbs
//     20 New glosses_with_prioritized displayed meaning of verbs_without_excel_formula_EN
//     21 New glosses (verbs:prioritized displayed meaning of verbs) with excel formula EN
//     22 meaning fixed
//     23 Peal=1
//     24 Pael=2
//     25 Peil=3
//     26 Hafel=4
//     27 Hofal=5
//     28 Afel=6
//     29 Shafel=7
//     30 Hitpeel=8
//     31 Hitpaal=9
//     32 Hishtafal=10
//     33 Etpaal=11
//     34 Etpeel=12
// Headers in output file:
//      0 Occurrences
//      1 lex
//      2 Lexeme
//      3 Transliterated
//      4 None
//      5 Peal
//      6 Peil
//      7 Pael
//      8 Hafel
//      9 Afel
//     10 Shafel
//     11 Hofal
//     12 Hitpeel
//     13 Hitpaal
//     14 Hishtafal
//     15 Etpeel
//     16 Etpaal

constexpr int MAX_OUT_FIELD = 17;

map<int,int> output2input = { {  1,  4 },   // lex
                              {  5, 23 },   // Peal     
                              {  6, 25 },   // Peil     
                              {  7, 24 },   // Pael     
                              {  8, 26 },   // Hafel    
                              {  9, 28 },   // Afel     
                              { 10, 29 },   // Shafel   
                              { 11, 27 },   // Hofal    
                              { 12, 30 },   // Hitpeel  
                              { 13, 31 },   // Hitpaal  
                              { 14, 32 },   // Hishtafal
                              { 15, 34 },   // Etpeel   
                              { 16, 33 } }; // Etpaal   


int main()
{
    string inputfilename{"ETCBC4-frequency4.02_progression-aram.csv"};
    string outputfilename{"aram_en.csv"};
    
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
                 {"Occurrences","lex","Lexeme","Transliterated","None","Peal","Peil","Pael",
                  "Hafel","Afel","Shafel","Hofal","Hitpeel","Hitpaal","Hishtafal","Etpeel","Etpaal"})
            outfile.SetColumnName(colno++,'"' + s + '"');

        // Convert each line
        for (size_t rix=0; rix<csvfile.GetRowCount(); ++rix) {
            vector<string> row = csvfile.GetRow<string>(rix);
        
            for (int cix=0; cix<MAX_OUT_FIELD; ++cix)
                if (output2input.count(cix)>0) {
                    string s = row[output2input[cix]];
                    outfile.SetCell<string>(cix, rix, s.empty() ? "" : '"' + row[output2input[cix]] + '"');
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
 
