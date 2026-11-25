#include "library_entity.h"

library_entity::library_entity() : id("") 
{}

library_entity::library_entity(std::string id) : id(id) 
{}

library_entity::~library_entity() 
{}

std::string library_entity::get_id() const {
    return id;
}

void library_entity::set_id(std::string id) {
    this->id = id;
}