#include "Utils.h"

std::map<OptionType, std::string> optionTypeNames = {
    {Call, "Call"},
    {Put, "Put"},
    {BinaryCall, "BinaryCall"},
    {BinaryPut, "BinaryPut"}
};
// Implementation of OptionTypeToString
std::string OptionTypeToString(OptionType type) {
    auto it = optionTypeNames.find(type);
    if (it != optionTypeNames.end()) {
        return it->second;
    }
    return "Unknown"; // Return "Unknown" if the type is not found
}
