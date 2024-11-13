#pragma once
#include <string> 
#include "Triangulation.h"
#include "Reader.h"

namespace IO {
    class STLReader : public Reader
    {
    public:
        STLReader();
        ~STLReader();

        // Reads the STL file specified by fileName and stores the triangulation data
        void read(const std::string& fileName, Triangulation& triangulation);

        bool operator()(double a, double b) const;
    };
}
