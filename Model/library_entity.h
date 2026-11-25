
#pragma once
#include <string>

// LỚP TRỪU TƯỢNG CƠ SỞ CHO MỌI THỰC THỂ TRONG THƯ VIỆN (ABSTRACTION)
class library_entity {
protected:
    std::string id;

public:
    library_entity();
    library_entity(std::string id);
    virtual ~library_entity();

    std::string get_id() const;
    void set_id(std::string id);

    // Các phương thức thuần ảo (Pure Virtual Functions) bắt buộc lớp con Override
    virtual std::string get_entity_type() const = 0;
    virtual std::string to_record_string() const = 0;
};
