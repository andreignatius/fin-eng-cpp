#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <string>

enum OptionType 
{   
    Call, 
    Put, 
    BinaryCall, 
    BinaryPut
};

enum class AssetType {
    Rate,
    Stock
};


#endif
