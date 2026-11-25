#pragma once
#include <string>

class role_permission {
public:
    virtual ~role_permission() = default;
    virtual std::string get_role_name() const = 0;
    virtual bool can_manage_users() const = 0;
};

class manager_permission : public role_permission {
public:
    std::string get_role_name() const override;
    bool can_manage_users() const override;
};

class admin_permission : public role_permission {
public:
    std::string get_role_name() const override;
    bool can_manage_users() const override;
};

class reader_permission : public role_permission {
public:
    std::string get_role_name() const override;
    bool can_manage_users() const override;
};

class permission_factory {
public:
    static const role_permission* create(const std::string& role_str);
};