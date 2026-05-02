#include <string>

#include "typelist.h"

using namespace typelist;

int main() {
    using List = TypeList<int, float, double, char>;

    static_assert(Size<List>::value == 4, "Size test failed");

    static_assert(std::is_same_v<TypeAt<List, 0>::type, int>, "TypeAt test 1 failed");
    static_assert(std::is_same_v<TypeAt<List, 2>::type, double>, "TypeAt test 2 failed");

    static_assert(Contains<float, List>::value, "Contains test 1 failed");
    static_assert(!Contains<void, List>::value, "Contains test 2 failed");

    static_assert(IndexOf<int, List>::value == 0, "IndexOf test 1 failed");
    static_assert(IndexOf<char, List>::value == 3, "IndexOf test 2 failed");

    using Appended = Append<List, void*>::type;
    static_assert(Size<Appended>::value == 5, "Append test failed");
    static_assert(std::is_same_v<TypeAt<Appended, 4>::type, void*>, "Append type test failed");

    using Prepended = Prepend<List, short>::type;
    static_assert(Size<Prepended>::value == 5, "Prepend test failed");
    static_assert(std::is_same_v<TypeAt<Prepended, 0>::type, short>, "Prepend type test failed");
    static_assert(std::is_same_v<TypeAt<Prepended, 1>::type, int>, "Prepend order test failed");

    // auto error1 = TypeAt<List, 10>::type;             // index out of range
    // auto error2 = IndexOf<std::string, List>::value;  // type not found
}