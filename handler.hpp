/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#ifndef _HANDLER_HPP
#define _HANDLER_HPP

#include <set>
#include <string>
#include <map>

class handler {
  public:
    // Adds the required features to s
    virtual void list_features(std::set<std::string>& s) {}
    
    // Analyzes contents of fmap
    virtual void prepare_object(std::map<std::string,std::string>& fmap) {}

    // Performs final calculations after parsing all feature maps
    virtual void finish_prepare() {}

    // Returns MQL code to be written before new features are generated
    virtual std::string pre_create() { return ""; }

    // Returns MQL code defining new features
    virtual std::string define_features() { return ""; }

    // Returns MQL code definining values of features in fmap
    virtual std::string update_object(const std::map<std::string,std::string>& fmap) { return ""; }
};

#endif // _HANDLER_HPP
