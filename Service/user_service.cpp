#include "user_service.h"

user_service::user_service(std::string filename) {
    data_file = filename;
    load_data();
}

user_service::~user_service() {
    save_data();
}

// --- Tách dòng từ user.txt (định dạng: ID;Name;Password;Role) ---
void user_service::parse_line(std::string line, user& u) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    std::stringstream ss(line);
    std::string id, name, password, role;

    std::getline(ss, id, ';');
    std::getline(ss, name, ';');
    std::getline(ss, password, ';');
    std::getline(ss, role, ';');

    u = user(id, name, password, role);
}

// --- 1. ĐỌC DỮ LIỆU TỪ FILE ---
bool user_service::load_data() {
    std::ifstream file(data_file);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        user u;
        parse_line(line, u);
        
        user_list.push_back(u);
        node<user>* new_node = user_list.get_tail(); 
        user_index.insert(u.get_id(), new_node); // Đăng ký bảng băm O(1)
    }
    file.close();
    return true;
}

// --- 2. GHI DỮ LIỆU XUỐNG FILE ---
bool user_service::save_data() {
    return file_helper::save_list_to_file(data_file, user_list);
}

// --- 3. XÁC THỰC ĐĂNG NHẬP ---
user* user_service::login(std::string id, std::string password) {
    node<user>* target = user_index.find(id);

    if (target == nullptr)
        return nullptr;

    if (target->get_Data().get_password() == password)
        return &target->get_Data();

    return nullptr; // sai mk
}

// --- Kiểm tra phân quyền (Hỗ trợ Manager > Admin > User) ---
bool user_service::check_permission_level(user* u, std::string required_role) {
    if (u == nullptr) return false;
    
    std::string role = u->get_role();
    if (role == "manager") return true; // Manager có quyền tối cao
    if (role == "admin" && (required_role == "admin" || required_role == "user")) return true;
    if (role == "user" && required_role == "user") return true;

    return false;
}

// --- 4. THÊM NGƯỜI DÙNG MỚI ---
bool user_service::add_user(user new_user) {
    if (user_index.find(new_user.get_id()) != nullptr) {
        return false; // Trùng ID tài khoản
    }

    user_list.push_back(new_user);
    user_index.insert(new_user.get_id(), user_list.get_tail());

    save_data(); // Lưu ngay xuống file
    return true;
}

// --- 5. XÓA NGƯỜI DÙNG ---
bool user_service::delete_user(std::string id) {
    node<user>* target = user_index.find(id);
    if (target == nullptr) return false;

    user_index.remove(id);
    user_list.remove_node(target); 

    save_data(); // Lưu ngay xuống file
    return true;
}

// --- 6. CẬP NHẬT THÔNG TIN NGƯỜI DÙNG ---
bool user_service::update_user(std::string old_id, user new_data) {
    node<user>* target = user_index.find(old_id);
    if (target == nullptr) return false;

    std::string new_id = new_data.get_id();

    if (old_id != new_id) {
        if (user_index.find(new_id) != nullptr) return false; 
        
        user_index.remove(old_id);             
        target->set_data(new_data);            
        user_index.insert(new_id, target);     
    } else {
        target->set_data(new_data);
    }

    save_data(); // Lưu ngay xuống file
    return true;
}

// --- 7. TRA CỨU NHANH THEO ID ---
node<user>* user_service::get_user_by_id(std::string id) {
    return user_index.find(id);
}