/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#include <utility>
#include <vector>
#include <cassert>
#include <iostream>
#include "util.hpp"

using namespace std;

// From http://stackoverflow.com/questions/5343190/how-do-i-replace-all-instances-of-of-a-string-with-another-string
void replace_string_in_place(string& subject, const string& search, const string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

// From http://stackoverflow.com/questions/874134/find-if-string-endswith-another-string-in-c
bool ends_with(const string& full_string, const string& ending)
{
    return full_string.length()>=ending.length() && 
        full_string.compare(full_string.length()-ending.length(), ending.length(), ending)==0;
}


int prev_u8_index(const string& s, int last_ix)
{
    while (--last_ix >= 0)
        if ((s[last_ix] & 0xc0) != 0x80) // Test for continuation char
            return last_ix;

    cerr << "prev_u8_index handled wrong string: '" << s << "'" << endl;
        
    return 0;
}

string trim_spaces(const string& s)
{
    string::const_iterator first, last;

    for (first=begin(s); first!=end(s) && *first==' '; ++first)
        /* nothing */;

    for (last=end(s); last!=begin(s) && *(last-1)==' '; --last)
        /* nothing */;

    return string{first,last};
}

static unsigned long u8tou32(const string& s, int index)
{
    unsigned char ch0 = s[index];
    unsigned char ch1 = s[index+1];

    if ((ch0 & 0x80) == 0)
        return ch0;

    assert((ch0 & 0xe0) == 0xc0); // We only handle two-byte UTF-8 sequences

    return ((ch0 & 0x1f) << 6) | (ch1 & 0x3f);
}

static void set_u8(string& s, int index, unsigned long u32)
{
    assert(u32 <= 0x7ff); // We only handle two-byte UTF-8 sequences

    s[index] = 0xc0 | ((u32 & 0x7c0)>>6);
    s[index+1] = 0x80 | (u32 & 0x3f);
}

string fix_final(string s)
{
    if (s.empty())
        return s;

    static vector<pair<unsigned long, unsigned long>> final_forms {
        make_pair(0x05db, 0x05da), // Kaph
        make_pair(0x05de, 0x05dd), // Mem
        make_pair(0x05e0, 0x05df), // Nun
        make_pair(0x05e4, 0x05e3), // Pe
        make_pair(0x05e6, 0x05e5), // Tsade
    };
            
    int index = static_cast<int>(s.length());

    do {
        index = prev_u8_index(s, index);
    
        auto u32 = u8tou32(s, index);

        if (u32>=0x05d0 /* Aleph */ && u32<=0x05ea /* Tav */) {
            for (auto f : final_forms) {
                if (u32 == f.first) {
                    set_u8(s, index, f.second);
                    return s; // Final consonant changed, we're done
                }
            }
            return s; // Final consonant did not need to be changed, we're done
        }
    } while (index>0);

    return s; // There was no final consonant
}
