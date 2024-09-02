#include "api/include/models/user.h"
#include "api/include/models/database.h"
#include <sqlite_modern_cpp.h>

namespace models {
    int User::create(const std::string &email, const std::string &password) {
        int user_id = 0;
        db << "INSERT INTO users (email, password) VALUES (?, ?);"
           << email
           << password;

        db << "SELECT last_insert_rowid();" >> user_id;
        return user_id;
    }

    bool User::authenticate(const std::string &email, const std::string &password) {
        bool user_found = false;
        db << "SELECT * FROM users WHERE email = ? AND password = ?;"
           << email
           << password
           >> [&](std::string, std::string) {
               user_found = true;
           };
        return user_found;
    }

    bool
    User::reset_password(const std::string &email, const std::string &old_password, const std::string &new_password) {
        bool is_changed = false;

        db << "UPDATE users SET password = ? WHERE email = ? AND password = ?;"
           << new_password
           << email
           << old_password;

        db << "SELECT changes();" >> [&](int changes) {
            if (changes > 0) { is_changed = true; }
        };
        return is_changed;
    }
}
