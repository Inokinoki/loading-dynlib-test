#include <vector>
#include <algorithm>

#define STATIC_LIB_DATA "static_lib_v2_data"

struct ShutdownData
{
    ~ShutdownData()
    {
        std::reverse(functions.begin(), functions.end());
        for (auto pair : functions)
            pair.first(pair.second);
    }

    static ShutdownData *get()
    {
        static auto *data = new ShutdownData;

        return data;
    }

    std::vector<std::pair<void (*)(const void *), const void *>> functions;
    std::string data = STATIC_LIB_DATA;
};

std::string fakeFunction()
{
    // This function is just a placeholder to demonstrate the use of the static library.
    ShutdownData *data = ShutdownData::get();
    return data->data;
}
