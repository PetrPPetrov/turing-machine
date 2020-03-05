// Copyright 2020 Petr Petrovich Petrov. All rights reserved.
// License: https://github.com/PetrPPetrov/turing-machine/blob/master/LICENSE

#include <cstddef>
#include <fstream>
#include "turing_machine.h"

int main(int argc, char** argv)
{
    std::cout << "Turing Machine Executor Copyright (c) 2020 Petr Petrovich Petrov" << std::endl;

    if (argc < 2)
    {
        std::cout << "usage: executor <binary_file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binary_file_name = argv[1];
    std::ifstream input_file(binary_file_name, std::ifstream::binary);
    if (!input_file)
    {
        std::cout << "Can not open binary file " << binary_file_name << std::endl;
        return EXIT_FAILURE;
    }

    input_file.seekg(0, input_file.end);
    std::size_t length = input_file.tellg();
    input_file.seekg(0, input_file.beg);

    if (length == 0)
    {
        std::cout << "The input file has zero size " << binary_file_name << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<DataType> buffer(length / sizeof(DataType));
    input_file.read(reinterpret_cast<char*>(&buffer[0]), buffer.size() * sizeof(DataType));
    input_file.close();

    return EXIT_SUCCESS;
}
