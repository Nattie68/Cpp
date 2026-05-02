#include <iostream>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>

class SetImpl {
public:
    virtual ~SetImpl() = default;
    
    virtual void add(int elem) = 0;
    virtual void remove(int elem) = 0;
    virtual bool contains(int elem) const = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
    
    virtual void forEach(std::function<void(int)> callback) const = 0;
    
    virtual void mergeFrom(const SetImpl& other) = 0;
    virtual void intersectFrom(const SetImpl& other) = 0;
};

class SmallSetImpl : public SetImpl {
private:
    std::vector<int> data;
    
    void sortAndUnique() {
        std::sort(data.begin(), data.end());
        auto last = std::unique(data.begin(), data.end());
        data.erase(last, data.end());
    }
    
public:
    void add(int elem) override {
        if (!contains(elem)) {
            data.push_back(elem);
        }
    }
    
    void remove(int elem) override {
        auto it = std::find(data.begin(), data.end(), elem);
        if (it != data.end()) {
            data.erase(it);
        }
    }
    
    bool contains(int elem) const override {
        return std::find(data.begin(), data.end(), elem) != data.end();
    }
    
    size_t size() const override {
        return data.size();
    }
    
    bool empty() const override {
        return data.empty();
    }
    
    void forEach(std::function<void(int)> callback) const override {
        for (int elem : data) {
            callback(elem);
        }
    }
    
    void mergeFrom(const SetImpl& other) override {
        other.forEach([this](int elem) {
            this->add(elem);
        });
    }
    
    void intersectFrom(const SetImpl& other) override {
        std::vector<int> newData;
        for (int elem : data) {
            if (other.contains(elem)) {
                newData.push_back(elem);
            }
        }
        data = std::move(newData);
    }
};

class LargeSetImpl : public SetImpl {
private:
    std::unordered_set<int> data;
    
public:
    void add(int elem) override {
        data.insert(elem);
    }
    
    void remove(int elem) override {
        data.erase(elem);
    }
    
    bool contains(int elem) const override {
        return data.find(elem) != data.end();
    }
    
    size_t size() const override {
        return data.size();
    }
    
    bool empty() const override {
        return data.empty();
    }
    
    void forEach(std::function<void(int)> callback) const override {
        for (int elem : data) {
            callback(elem);
        }
    }
    
    void mergeFrom(const SetImpl& other) override {
        other.forEach([this](int elem) {
            this->add(elem);
        });
    }
    
    void intersectFrom(const SetImpl& other) override {
        std::unordered_set<int> newData;
        for (int elem : data) {
            if (other.contains(elem)) {
                newData.insert(elem);
            }
        }
        data = std::move(newData);
    }
};

class Set {
private:
    std::unique_ptr<SetImpl> impl;
    static constexpr size_t THRESHOLD = 10;
    
    void transferData(SetImpl* from, SetImpl* to) {
        from->forEach([to](int elem) {
            to->add(elem);
        });
    }
    
    void checkAndSwitch() {
        size_t currentSize = impl->size();
        
        if (currentSize > THRESHOLD && dynamic_cast<SmallSetImpl*>(impl.get())) {
            auto newImpl = std::make_unique<LargeSetImpl>();
            transferData(impl.get(), newImpl.get());
            impl = std::move(newImpl);
        }
        else if (currentSize <= THRESHOLD && dynamic_cast<LargeSetImpl*>(impl.get())) {
            auto newImpl = std::make_unique<SmallSetImpl>();
            transferData(impl.get(), newImpl.get());
            impl = std::move(newImpl);
        }
    }
    
public:
    Set() : impl(std::make_unique<SmallSetImpl>()) {}
    
    void add(int elem) {
        impl->add(elem);
        checkAndSwitch();
    }
    
    void remove(int elem) {
        impl->remove(elem);
        checkAndSwitch();
    }
    
    bool contains(int elem) const {
        return impl->contains(elem);
    }
    
    size_t size() const {
        return impl->size();
    }
    
    bool empty() const {
        return impl->empty();
    }
    
    void merge(const Set& other) {
        impl->mergeFrom(*other.impl);
        checkAndSwitch();
    }
    
    void intersect(const Set& other) {
        impl->intersectFrom(*other.impl);
        checkAndSwitch();
    }
    
    void print() const {
        std::cout << "{ ";
        impl->forEach([](int elem) {
            std::cout << elem << " ";
        });
        std::cout << "} (size: " << impl->size() << ", type: ";
        
        if (dynamic_cast<const SmallSetImpl*>(impl.get())) {
            std::cout << "SmallSet";
        } else if (dynamic_cast<const LargeSetImpl*>(impl.get())) {
            std::cout << "LargeSet";
        }
        std::cout << ")" << std::endl;
    }
    
    std::string getImplType() const {
        if (dynamic_cast<const SmallSetImpl*>(impl.get())) {
            return "SmallSet";
        } else if (dynamic_cast<const LargeSetImpl*>(impl.get())) {
            return "LargeSet";
        }
        return "Unknown";
    }
};

int main() {
    Set set1;
    std::cout << "Empty set:" << std::endl;
    set1.print();
    
    std::cout << "Add elements up to 10:" << std::endl;
    for (int i = 1; i <= 10; i++) {
        set1.add(i);
    }
    set1.print();
    
    std::cout << "Add 11th element (switch to LargeSet):" << std::endl;
    set1.add(11);
    set1.print();
    
    std::cout << "Add elements up to 15:" << std::endl;
    for (int i = 12; i <= 15; i++) {
        set1.add(i);
    }
    set1.print();
    
    std::cout << "Erase elements down to 10 (switch to SmallSet):" << std::endl;
    for (int i = 11; i <= 15; i++) {
        set1.remove(i);
    }
    set1.print();
    
    std::cout << "Sets merge" << std::endl;
    Set setA;
    for (int i = 1; i <= 5; i++) setA.add(i);
    
    Set setB;
    for (int i = 4; i <= 15; i++) setB.add(i);
    
    std::cout << "Set A: ";
    setA.print();
    std::cout << "Set B: ";
    setB.print();
    
    setA.merge(setB);
    std::cout << "Merge of A with B: ";
    setA.print();
    
    std::cout << "Sets intersection" << std::endl;
    Set setC;
    for (int i = 1; i <= 10; i++) setC.add(i);
    
    Set setD;
    for (int i = 5; i <= 15; i++) setD.add(i);
    
    std::cout << "Set C: ";
    setC.print();
    std::cout << "Set D: ";
    setD.print();
    
    setC.intersect(setD);
    std::cout << "Intersection of C and D: ";
    setC.print();
    
    return 0;
}