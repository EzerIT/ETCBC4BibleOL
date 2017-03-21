/* Copyright © 2017 Oliver Glanz and Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#include <cassert>
#include <iostream>
#include <map>

#include "verb_class_handler.hpp"
#include "verb_class.hpp"


using namespace std;

class verb_class_handler : public handler {
  public:
    virtual void list_features(set<string>&) override;
    virtual string pre_create() override;
    virtual string define_features() override;
    virtual string update_object(const map<string,string>&) override;

  private:
    string normalize(const string& verb);
    string classify_aramaic(const string& verb);
    string classify_hebrew(const string& verb);
};

shared_ptr<handler> make_verb_class_handler()
{
    return shared_ptr<handler>{new verb_class_handler};
}


void verb_class_handler::list_features(set<string>& req)
{
    build_newgloss();
    req.insert({"lex","sp","language"});
}


string verb_class_handler::pre_create()
{
    return
        "CREATE ENUMERATION verb_class_t = {\n"
        "    regular,\n"
        "    i_guttural,\n"
        "    ii_guttural,\n"
        "    iii_guttural,\n"
        "    i_aleph,\n"
        "    iii_aleph,\n"
        "    iii_he,\n"
        "    i_nun,\n"
        "    analog_i_nun,\n"
        "    i_waw,\n"
        "    analog_i_waw,\n"
        "    i_yod,\n"
        "    ii_waw,\n"
        "    ii_yod,\n"
        "    four_consonants,\n"
        "    geminate,\n"
        "    hjh_xjh\n"
        "} GO\n";
}

string verb_class_handler::define_features()
{
    return "    ADD verb_class : LIST OF verb_class_t;\n";
}

string verb_class_handler::normalize(const string& verb)
{
    if (verb.length()<4) {
        cerr << "Verb " << verb << " is too short\n";
        return verb;
    }

    if (verb.substr(verb.length()-4)=="===[")
        return verb.substr(0,verb.length()-4);

    if (verb.substr(verb.length()-3)=="==[")
        return verb.substr(0,verb.length()-3);

    if (verb.substr(verb.length()-2)=="=[")
        return verb.substr(0,verb.length()-2);

    if (verb.substr(verb.length()-1)=="[")
        return verb.substr(0,verb.length()-1);

    cerr << "Verb " << verb << " does not end in [\n";
    return verb;
}


string verb_class_handler::classify_aramaic(const string& verb)
{
    string norm = normalize(verb);
    string attr;
    bool is_pjod = false;

    if (norm.length() == 3) {
        if (norm==">MR") {
            if (verb==">MR=[")
                attr += ",i_guttural";
            else if (verb==">MR[")
                attr += ",i_aleph";
            else
                cerr << "Verb " << verb << " is not recognized\n";
        }
        else if (norm==">KL" ||
                 norm==">BD") {
            attr += ",i_aleph";

        }
        else if (norm==">BH" ||
                 norm==">PH") {
            attr += ",i_aleph,iii_he";
        }
        else if (norm=="HJH" ||
                 norm=="XJH") {
            attr += ",iii_he,i_guttural";
        }
        else {
            if (norm[0]=='N') {
                attr += ",i_nun";
            }
            if (norm[0]=='J') {
                if (norm=="JVB" ||
                    norm=="JLL" ||
                    norm=="JMN" ||
                    norm=="JNQ" ||
                    norm=="JQY" ||
                    norm=="JBC" ||
                    norm=="JCR") {
                    attr += ",i_yod";
                    is_pjod = true;
                }
                else if (norm=="JCN") {
                    if (verb=="JCN=[")
                        attr += ",i_waw";
                    else if (verb=="JCN[") {
                        attr += ",i_yod";
                        is_pjod = true;
                    }
                    else
                        cerr << "Verb " << verb << " is not recognized\n";
                }
                else {
                    attr += ",i_waw";
                }
            }

            if (norm=="HLK") {
                attr += ",i_waw";
            }

            if (norm[1]=='J' && norm[2]!='H') {
                attr += ",ii_yod";
            }

            if (norm[1]=='W' && norm[2]!='H') {
                attr += ",ii_waw";
            }

            if (norm[1]==norm[2] && !is_pjod) {
                attr += ",geminate";
            }

            if (norm[2]=='>') {
                attr += ",iii_aleph";
            }

            if (norm[2]=='H') {
                attr += ",iii_he";
            }

            if (norm[0]=='>' ||
                norm[0]=='<' ||
                norm[0]=='R' ||
                norm[0]=='H' ||
                norm[0]=='X') {
                attr += ",i_guttural";
            }

            if (norm[1]=='>' ||
                norm[1]=='<' ||
                norm[1]=='R' ||
                norm[1]=='H' ||
                norm[1]=='X') {
                attr += ",ii_guttural";
            }

            if (norm[2]=='<' ||
                norm[2]=='R' ||
                norm[2]=='X') {
                attr += ",iii_guttural";
            }
        }
    }
    else {
        attr += ",four_consonants";
    }
    return attr.length()>0 ? attr.substr(1) : "regular";
}

string verb_class_handler::classify_hebrew(const string& verb)
{
    assert(verb.substr(verb.length()-1)=="[");

    set<string> vc = verbclasses_lookup(verb);

    string attr;

    for (auto x : vc) {
        if (x=="4 root verb")       attr += ",four_consonants";
        else if (x=="geminate")     attr += ",geminate";
        else if (x=="i-aleph")      attr += ",i_aleph";
        else if (x=="i-guttural")   attr += ",i_guttural";
        else if (x=="i-nun")        attr += ",i_nun";
        else if (x=="i-waw")        attr += ",i_waw";
        else if (x=="i-yod")        attr += ",i_yod";
        else if (x=="ii-guttural")  attr += ",ii_guttural";
        else if (x=="ii-waw")       attr += ",ii_waw";
        else if (x=="ii-yod")       attr += ",ii_yod";
        else if (x=="iii-aleph")    attr += ",iii_aleph";
        else if (x=="iii-guttural") attr += ",iii_guttural";
        else if (x=="iii-hey")      attr += ",iii_he";
        else if (x=="regular")      attr += ",regular";
    }
    
    return attr.length()>0 ? attr.substr(1) : "";
}

string verb_class_handler::update_object(const map<string,string>& fmap)
{
    if (fmap.at("sp")!="verb")
        return "";

    if (fmap.at("language")=="Aramaic")
        return "    verb_class := (" + classify_aramaic(fmap.at("lex")) + ");\n";
    else
        return "    verb_class := (" + classify_hebrew(fmap.at("lex")) + ");\n";
}
