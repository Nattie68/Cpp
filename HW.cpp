#include <fstream>
#include <string>
#include <vector>

class FileWriter {
private:
    std::ofstream file;
    std::string filename;
    bool first_field;

public:
    FileWriter(const std::string& fname) : filename(fname), first_field(true) {
        file.open(filename, std::ios::app);
        file << "{\n";
    }
    
    ~FileWriter() {
        if (file.is_open()) {
            file << "\n}";
            file.close();
        }
    }
    
    FileWriter(FileWriter&& other) noexcept 
        : file(std::move(other.file)), 
          filename(std::move(other.filename)),
          first_field(other.first_field) {
        other.first_field = true;
    }
    
    FileWriter& operator=(FileWriter&& other) noexcept {
        if (this != &other) {
            file = std::move(other.file);
            filename = std::move(other.filename);
            first_field = other.first_field;
            other.first_field = true;
        }
        return *this;
    }
    
    FileWriter(const FileWriter&) = delete;
    FileWriter& operator=(const FileWriter&) = delete;
    
    void Add(const std::string& key, const std::string& value) {
        if (!first_field) {
            file << ",\n";
        }
        file << "  \"" << key << "\": \"" << value << "\"";
        first_field = false;
    }
};

class Entity {
private:
    FileWriter writer;

public:
    Entity(const std::string& filename) : writer(filename) {}
    
    ~Entity() = default;
    
    Entity(Entity&& other) noexcept : writer(std::move(other.writer)) {}
    
    Entity& operator=(Entity&& other) noexcept {
        if (this != &other) {
            writer = std::move(other.writer);
        }
        return *this;
    }
    
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    
    void Add(const std::string& key, const std::string& value) {
        writer.Add(key, value);
    }
};

int main() {
    Entity entity1("data.json");
    entity1.Add("name", "John");
    entity1.Add("age", "30");
    
    Entity entity2("data2.json");
    entity2.Add("city", "New York");
    entity2.Add("country", "USA");
    
    Entity entity3 = std::move(entity1);
    entity3.Add("job", "Engineer");
    entity3.Add("surname", "Connor");
    
    return 0;
}