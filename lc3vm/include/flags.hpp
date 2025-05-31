#ifndef LC3_FLAGS_H
#define LC3_FLAGS_H

enum StatusFlags {
    FL_POS = 1 << 0,  // Positive
    FL_ZRO = 1 << 1,  // Zero
    FL_NEG = 1 << 2   // Negative
};

#endif // LC3_FLAGS_H