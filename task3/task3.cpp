#include <iostream>
#include <string>
#include <tuple>
#include <optional>

#include "typelist.h"

using namespace typelist;

template<typename... Types>
class TypeMap {
private:
    using KeyList = TypeList<Types...>;
    
    template<typename List>
    struct Storage;
    
    template<typename... Ts>
    struct Storage<TypeList<Ts...>> {
        std::tuple<std::optional<Ts>...> data;
    };
    
    Storage<KeyList> storage;
    
    template<typename T>
    static constexpr std::size_t GetIndex() {
        return IndexOf<T, KeyList>::value;
    }
    
    template<std::size_t Index>
    auto& getOptional() {
        return std::get<Index>(storage.data);
    }
    
public:
    template<typename T>
    void AddValue(const T& value) {
        constexpr std::size_t idx = GetIndex<T>();
        getOptional<idx>() = value;
    }
    
    template<typename T>
    T& GetValue() {
        constexpr std::size_t idx = GetIndex<T>();
        return getOptional<idx>().value();
    }
    
    template<typename T>
    bool Contains() const {
        constexpr std::size_t idx = GetIndex<T>();
        return std::get<idx>(storage.data).has_value();
    }
    
    template<typename T>
    void RemoveValue() {
        constexpr std::size_t idx = GetIndex<T>();
        getOptional<idx>().reset();
    }
};

struct DataA {
    std::string value;
};

struct DataB {
    int value;
};

int main() {
    TypeMap<int, DataA, double, DataB> myTypeMap;
    
    // Добавление элементов в контейнер
    myTypeMap.AddValue<int>(42);
    myTypeMap.AddValue<double>(3.14);
    myTypeMap.AddValue<DataA>({"Hello, TypeMap!"});
    myTypeMap.AddValue<DataB>({10});
    
    // Получение и вывод значений по типам ключей
    std::cout << "Value for int: " << myTypeMap.GetValue<int>() << std::endl;
    std::cout << "Value for double: " << myTypeMap.GetValue<double>() << std::endl;
    std::cout << "Value for DataA: " << myTypeMap.GetValue<DataA>().value << std::endl;
    std::cout << "Value for DataB: " << myTypeMap.GetValue<DataB>().value << std::endl;
    
    // Проверка наличия элемента
    std::cout << "Contains int? " << (myTypeMap.Contains<int>() ? "Yes" : "No") << std::endl;
    
    // Удаление элемента
    myTypeMap.RemoveValue<double>();
    
    // Попытка получения удаленного элемента
    std::cout << "Value for double after removal: " << myTypeMap.GetValue<double>() << std::endl;
    
    return 0;
}