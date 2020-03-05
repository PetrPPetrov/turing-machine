// Copyright 2020 Petr Petrovich Petrov. All rights reserved.
// License: https://github.com/PetrPPetrov/turing-machine/blob/master/LICENSE

#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <limits>
#include "turing_machine.h"

std::map<std::string, DataType> g_label_to_address;

std::set<std::string> g_input_variables;
std::map<std::string, DataType> g_input_arrays;

std::set<std::string> g_output_variables;
std::map<std::string, DataType> g_output_arrays;

std::vector<DataType> g_program;

bool isSeparator(char a)
{
    return a == ' ' || a == '\t';
}

bool isOneCharToken(char a)
{
    return a == '[' || a == ']';
}

std::string nextToken(std::string& line)
{
    std::string result;
    std::size_t i = 0;
    while (i < line.size() && isSeparator(line[i]))
    {
        ++i;
    }
    if (isOneCharToken(line[i]))
    {
        result = line[i];
        ++i;
    }
    else
    {
        while (i < line.size() && !isSeparator(line[i]) && !isOneCharToken(line[i]))
        {
            result += line[i];
            ++i;
        }
    }
    line = line.substr(i);
    return result;
}

void parseLabel(std::string line, std::size_t line_number)
{
    std::string label = nextToken(line);
    if (label.empty())
    {
        throw std::runtime_error("Label is empty (" + std::to_string(line_number) + ")");
    }
    if (g_label_to_address.find(label) != g_label_to_address.end())
    {
        throw std::runtime_error("Label is already defined, '" + label + "' (" + std::to_string(line_number) + ")");
    }
    g_label_to_address.emplace(label, static_cast<DataType>(g_program.size()));
    std::cout << "label " << label << std::endl;
}

void parseInput(std::string line, std::size_t line_number)
{
    std::string keyword = nextToken(line);
    if (keyword == "variable")
    {
        std::string name = nextToken(line);
        if (name.empty())
        {
            throw std::runtime_error("Variable name is empty (" + std::to_string(line_number) + ")");
        }
        if (g_input_variables.find(name) != g_input_variables.end())
        {
            throw std::runtime_error("Input variable is already declared, '" + name + "' (" + std::to_string(line_number) + ")");
        }
        g_input_variables.emplace(name);
        std::cout << "input variable " << name << std::endl;
    }
    else if (keyword == "array")
    {
        std::string name = nextToken(line);
        if (name.empty())
        {
            throw std::runtime_error("Array name is empty (" + std::to_string(line_number) + ")");
        }
        if (g_input_arrays.find(name) != g_input_arrays.end())
        {
            throw std::runtime_error("Input array is already declared, '" + name + "' (" + std::to_string(line_number) + ")");
        }
        keyword = nextToken(line);
        if (keyword != "[")
        {
            throw std::runtime_error("Expected '[' after input array name declaration (" + std::to_string(line_number) + ")");
        }
        std::string size = nextToken(line);
        DataType array_size = std::stoi(size);
        if (array_size >= std::numeric_limits<DataType>::max())
        {
            throw std::runtime_error("Input array has too big size, '" + name + "' (" + std::to_string(line_number) + ")");
        }
        keyword = nextToken(line);
        if (keyword != "]")
        {
            throw std::runtime_error("Expected ']' after input array size declaration (" + std::to_string(line_number) + ")");
        }
        g_input_arrays.emplace(name, array_size);
        std::cout << "input array " << name << "[" << std::to_string(array_size) << "]" << std::endl;
    }
}

void parseLine(std::string line, std::size_t line_number)
{
    while (!line.empty())
    {
        std::string keyword = nextToken(line);
        if (keyword == "label")
        {
            parseLabel(line, line_number);
        }
        else if (keyword == "input")
        {
            parseInput(line, line_number);
        }
        else if (keyword == "let")
        {

        }
    }
}

int main(int argc, char** argv)
{
    std::cout << "Turing Machine Compiler Copyright (c) 2020 Petr Petrovich Petrov" << std::endl;

    if (argc < 3)
    {
        std::cout << "usage: compiler <source_file.zx> <binary_output_file.b16>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string source_file_name = argv[1];
    std::string binary_file_name = argv[2];

    std::ifstream input_file(source_file_name);
    if (!input_file)
    {
        std::cout << "Can not open source file " << source_file_name << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        std::size_t line_number = 1;
        while (!input_file.eof())
        {
            char buffer[256] = { 0 };
            input_file.getline(buffer, 255);
            std::string line(buffer);
            parseLine(line, line_number++);
        }
    }
    catch (const std::exception& compilation_error)
    {
        std::cout << "Compilation error: " << compilation_error.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
