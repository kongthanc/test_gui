
#pragma once
#include <string>
#include <iostream>
#include "library_entity.h"
#include "role_permission.h"

class user : public library_entity {
private:
    std::string name;
    std::string password;
    std::string role;
    role_permission* permission_strategy; // Con trá» Äa hÃ¬nh (Strategy Pattern)

    void update_permission_strategy();

public:
    user();
    user(std::string id, std::string name, std::string password, std::string role);
    user(const user& other);
    user& operator=(const user& other);
    virtual ~user();

    std::string get_name() const;
    std::string get_password() const;
    std::string get_role() const;

    void set_name(std::string name);
    void set_password(std::string password);
    void set_role(std::string role);

    // CÃ¡c hÃ nh vi Äa hÃ¬nh (Polymorphic Dispatch)
    void display_role_menu() const;
    int get_max_menu_option() const;
    bool has_user_management_privilege() const;

    // Ghi Ä‘Ã¨ (Override) hÃ m thuáº§n áº£o tá»« lá»›p trá»«u tÆ°á»£ng library_entity
    std::string get_entity_type() const override;
    std::string to_record_string() const override;

    friend std::istream& operator>>(std::istream& in, user& u);
    friend std::ostream& operator<<(std::ostream& out, const user& u);
};
