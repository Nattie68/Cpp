#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>

using Id = uint64_t;

class Group;

class User {
public:
    User(Id id, std::string name)
        : id_(id), name_(std::move(name)), group_() {}

    Id getId() const { return id_; }
    const std::string& getName() const { return name_; }

    void setGroup(std::weak_ptr<Group> group) { group_ = group; }
    std::shared_ptr<Group> getGroup() const { return group_.lock(); }

private:
    Id id_;
    std::string name_;
    std::weak_ptr<Group> group_;
};

class Group : public std::enable_shared_from_this<Group> { 
public:
    Group(Id id) : id_(id) {}

    Id getId() const { return id_; }

    void addUser(std::shared_ptr<User> user) {
        users_[user->getId()] = user;
        user->setGroup(shared_from_this());
    }

    void removeUser(Id userId) {
        auto it = users_.find(userId);
        if (it != users_.end()) {
            if (auto user = it->second) {
                user->setGroup({}); 
            }
            users_.erase(it);
        }
    }

    const std::unordered_map<Id, std::shared_ptr<User>>& getUsers() const {
        return users_;
    }

private:
    Id id_;
    std::unordered_map<Id, std::shared_ptr<User>> users_;
};

class UserGroupManager {
public:
    void createUser(Id userId, const std::string& username) {
        if (users_.count(userId)) {
            std::cout << "User with ID: " << userId << " already exists" << std::endl;
            return;
        }
        users_[userId] = std::make_shared<User>(userId, username);
        std::cout << "User created with ID: " << userId << std::endl;
    }

    void deleteUser(Id userId) {
        auto it = users_.find(userId);
        if (it != users_.end()) {
            if (auto group = it->second->getGroup()) {
                group->removeUser(userId);
            }
            users_.erase(it);
            std::cout << "User " << userId << " deleted" << std::endl;
        } else {
            std::cout << "User not found" << std::endl;
        }
    }

    void allUsers() const {
        if (users_.empty()) {
            std::cout << "No users" << std::endl;
            return;
        }
        for (const auto& [id, user] : users_) {
            std::cout << "User ID: " << id 
                    << ", Name: " << user->getName();
            if (auto group = user->getGroup()) {
                std::cout << ", Group: " << group->getId();
            }
            std::cout << std::endl;
        }
    }

    void getUser(Id userId) const {
        auto it = users_.find(userId);
        if (it != users_.end()) {
            std::cout << "User ID: " << userId 
                    << ", Name: " << it->second->getName();
            if (auto group = it->second->getGroup()) {
                std::cout << ", Group: " << group->getId();
            }
            std::cout << std::endl;
        } else {
            std::cout << "User not found" << std::endl;
        }
    }

    void createGroup(Id groupId) {
        if (groups_.count(groupId)) {
            std::cout << "Group with ID: " << groupId << " already exists" << std::endl;
            return;
        }
        groups_[groupId] = std::make_shared<Group>(groupId);
        std::cout << "Group created with ID: " << groupId << std::endl;
    }

    void deleteGroup(Id groupId) {
        auto it = groups_.find(groupId);
        if (it != groups_.end()) {
            for (const auto& [uid, user] : it->second->getUsers()) {
                user->setGroup({});
            }
            groups_.erase(it);
            std::cout << "Group " << groupId << " deleted" << std::endl;
        } else {
            std::cout << "Group not found" << std::endl;
        }
    }

    void allGroups() const {
        if (groups_.empty()) {
            std::cout << "No groups" << std::endl;
            return;
        }
        for (const auto& [gid, group] : groups_) {
            std::cout << "Group ID: " << gid << std::endl;
            const auto& users = group->getUsers();
            if (users.empty()) {
                std::cout << "  (no users)" << std::endl;
            } else {
                for (const auto& [uid, user] : users) {
                    std::cout << "  User: " << uid << " (" << user->getName() << ")" << std::endl;
                }
            }
        }
    }

    void getGroup(Id groupId) const {
        auto it = groups_.find(groupId);
        if (it != groups_.end()) {
            std::cout << "Group ID: " << groupId << std::endl;
            const auto& users = it->second->getUsers();
            if (users.empty()) {
                std::cout << "  (no users)" << std::endl;
            } else {
                for (const auto& [uid, user] : users) {
                    std::cout << "  User: " << uid << " (" << user->getName() << ")" << std::endl;
                }
            }
        } else {
            std::cout << "Group not found" << std::endl;
        }
    }

    void addUserToGroup(Id userId, Id groupId) {
        auto userIt = users_.find(userId);
        auto groupIt = groups_.find(groupId);
        
        if (userIt == users_.end()) {
            std::cout << "User not found" << std::endl;
            return;
        }
        if (groupIt == groups_.end()) {
            std::cout << "Group not found" << std::endl;
            return;
        }
        
        if (auto oldGroup = userIt->second->getGroup()) {
            oldGroup->removeUser(userId);
            std::cout << "User " << userId << " removed from group " << oldGroup->getId() << std::endl;
        }
        
        groupIt->second->addUser(userIt->second);
        std::cout << "User " << userId << " added to group " << groupId << std::endl;
    }

    void removeUserFromGroup(Id userId, Id groupId) {
        auto groupIt = groups_.find(groupId);
        if (groupIt != groups_.end()) {
            groupIt->second->removeUser(userId);
            std::cout << "User " << userId << " removed from group " << groupId << std::endl;
        } else {
            std::cout << "Group not found" << std::endl;
        }
    }

private:
    std::unordered_map<Id, std::shared_ptr<User>> users_;
    std::unordered_map<Id, std::shared_ptr<Group>> groups_;
};

int main() {
    UserGroupManager manager;
    std::string command;
    
    std::cout << "User/Group Management System" << std::endl;
    std::cout << "Commands: createUser, deleteUser, allUsers, getUser, createGroup, deleteGroup, allGroups, getGroup, addUserToGroup, removeUserFromGroup, exit" << std::endl;
    
    while (true) {
        std::cout << "> ";
        std::cin >> command;
        
        if (command == "exit") break;
        else if (command == "createUser") {
            Id id;
            std::cout << "Enter user ID: ";
            std::cin >> id;
            std::string name;
            std::cout << "Enter username: ";
            std::cin >> name;
            manager.createUser(id, name);
        }
        else if (command == "deleteUser") {
            Id id;
            std::cout << "Enter user ID to delete: ";
            std::cin >> id;
            manager.deleteUser(id);
        }
        else if (command == "allUsers") {
            manager.allUsers();
        }
        else if (command == "getUser") {
            Id id;
            std::cout << "Enter user ID: ";
            std::cin >> id;
            manager.getUser(id);
        }
        else if (command == "createGroup") {
            Id id;
            std::cout << "Enter group ID: ";
            std::cin >> id;
            manager.createGroup(id);
        }
        else if (command == "deleteGroup") {
            Id id;
            std::cout << "Enter group ID to delete: ";
            std::cin >> id;
            manager.deleteGroup(id);
        }
        else if (command == "allGroups") {
            manager.allGroups();
        }
        else if (command == "getGroup") {
            Id id;
            std::cout << "Enter group ID: ";
            std::cin >> id;
            manager.getGroup(id);
        }
        else if (command == "addUserToGroup") {
            Id userId, groupId;
            std::cout << "Enter user ID: ";
            std::cin >> userId;
            std::cout << "Enter group ID: ";
            std::cin >> groupId;
            manager.addUserToGroup(userId, groupId);
        }
        else if (command == "removeUserFromGroup") {
            Id userId, groupId;
            std::cout << "Enter user ID: ";
            std::cin >> userId;
            std::cout << "Enter group ID: ";
            std::cin >> groupId;
            manager.removeUserFromGroup(userId, groupId);
        }
        else {
            std::cout << "Unknown command" << std::endl;
        }
    }
    
    return 0;
}