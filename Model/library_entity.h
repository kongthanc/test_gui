
#pragma once
#include <string>

// Lá»šP TRá»ªU TÆ¯á»¢NG CÆ  Sá»ž CHO Má»ŒI THá»°C THá»‚ TRONG THÆ¯ VIá»†N (ABSTRACTION)
class library_entity {
protected:
    std::string id;

public:
    library_entity() : id("") {}
    library_entity(std::string id) : id(id) {}
    virtual ~library_entity() {}

    std::string get_id() const { return id; }
    void set_id(std::string id) { this->id = id; }  

    // CÃ¡c phÆ°Æ¡ng thá»©c thuáº§n áº£o (Pure Virtual Functions) báº¯t buá»™c lá»›p con Override
    virtual std::string get_entity_type() const = 0;
    virtual std::string to_record_string() const = 0;
};
