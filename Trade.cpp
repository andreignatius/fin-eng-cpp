#include "Trade.h"

// Accessor for the type of trade
string Trade::getType() const {
    return tradeType;
}

// Accessor for the underlying asset
string Trade::getUnderlying() const {
    return underlying;
}

