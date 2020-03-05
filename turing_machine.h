// Copyright 2020 Petr Petrovich Petrov. All rights reserved.
// License: https://github.com/PetrPPetrov/turing-machine/blob/master/LICENSE

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <iostream>

typedef std::uint16_t DataType;
typedef std::uint32_t MaxDataType;

enum class EOpCode : DataType
{
    Nop,
    Add, // V[a] = V[b] + V[c]
    Sub, // V[a] = V[b] - V[c]
    Mul, // V[a] = V[b] * V[c]
    Div, // V[a] = V[b] / V[c]
    Mod, // V[a] = V[b] % V[c]
    IfALessB,        // if (V[a] < V[b]) goto label1;
    IfALessEqB,      // if (V[a] <= V[b]) goto label1;
    IfAEqualB,       // if (V[a] == V[b]) goto label1;
    IfANotEqualB,    // if (V[a] != V[b]) goto label1;
    IfAGreaterB,     // if (V[a] > V[b]) goto label1;
    IfAGreaterEqB,   // if (V[a] >= V[b]) goto label1;
    Goto,            // goto label1;
    IndirectGoto,    // goto V[a];
    Allocate,        // alloc(V[a]);
    Stop             // Successfully stops the program
};

#pragma pack(push, 1)

struct Instruction
{
    EOpCode op_code;
};

struct NopInstruction : public Instruction
{
};

struct BinaryOperationAndAssignmentInstruction : public Instruction
{
    DataType flags;
    DataType result_index;
    DataType a_index_or_value;
    DataType b_index_or_value;
};

struct IfInstruction : public Instruction
{
    DataType flags;
    DataType a_index_or_value;
    DataType b_index_or_value;
    DataType new_address;
};

struct GotoInstruction : public Instruction
{
    DataType new_address_or_label_index;
};

struct AllocateInstruction : public Instruction
{
    DataType variable_index;
};

#pragma pack(pop)

inline bool executeNop(std::vector<DataType>& memory, std::size_t& ip)
{
    ip += sizeof(NopInstruction);
    return false;
}

inline bool executeBinaryOperationAndAssignment(std::vector<DataType>& memory, std::size_t& ip)
{
    if (memory.size() >= ip + sizeof(BinaryOperationAndAssignmentInstruction))
    {
        BinaryOperationAndAssignmentInstruction* instruction = reinterpret_cast<BinaryOperationAndAssignmentInstruction*>(&memory[ip]);
        DataType argument_a = instruction->a_index_or_value;
        DataType argument_b = instruction->b_index_or_value;
        if (instruction->flags & 0x1) // bit meaning: 0 means constant, 1 means index of variable
        {
            if (argument_a >= memory.size())
            {
                std::cout << "Access violation on read 0x" << std::hex << static_cast<MaxDataType>(argument_a) << std::endl;
                return true;
            }
            argument_a = memory[argument_a];
        }
        if (instruction->flags & 0x2) // bit meaning: 0 means constant, 1 means index of variable
        {
            if (argument_b >= memory.size())
            {
                std::cout << "Access violation on read 0x" << std::hex << static_cast<MaxDataType>(argument_b) << std::endl;
                return true;
            }
            argument_b = memory[argument_b];
        }
        DataType result = 0;
        switch (instruction->op_code)
        {
        case EOpCode::Add:
            result = argument_a + argument_b;
            break;
        case EOpCode::Sub:
            result = argument_a - argument_b;
            break;
        case EOpCode::Mul:
            result = argument_a * argument_b;
            break;
        case EOpCode::Div:
            if (argument_b == 0)
            {
                std::cout << "Division by zero" << std::endl;
                return true;
            }
            result = argument_a / argument_b;
            break;
        case EOpCode::Mod:
            if (argument_b == 0)
            {
                std::cout << "Division by zero" << std::endl;
                return true;
            }
            result = argument_a % argument_b;
            break;
        default:
            std::cout << "Invalid opcode 0x" << std::hex << static_cast<MaxDataType>(instruction->op_code) << std::endl;
            return true;
        }
        
        if (instruction->result_index >= memory.size())
        {
            std::cout << "Access violation on write 0x" << std::hex << static_cast<MaxDataType>(argument_a) << std::endl;
            return true;
        }
        memory[instruction->result_index] = result;
        ip += sizeof(BinaryOperationAndAssignmentInstruction);
        return false;
    }
    std::cout << "Instruction is not complete" << std::endl;
    return true;
}

inline bool executeIf(std::vector<DataType>& memory, std::size_t& ip)
{
    if (memory.size() >= ip + sizeof(IfInstruction))
    {
        IfInstruction* instruction = reinterpret_cast<IfInstruction*>(&memory[ip]);
        DataType argument_a = instruction->a_index_or_value;
        DataType argument_b = instruction->b_index_or_value;
        if (instruction->flags & 0x1) // bit meaning: 0 means constant, 1 means index of variable
        {
            if (argument_a >= memory.size())
            {
                std::cout << "Access violation on read 0x" << std::hex << static_cast<MaxDataType>(argument_a) << std::endl;
                return true;
            }
            argument_a = memory[argument_a];
        }
        if (instruction->flags & 0x2) // bit meaning: 0 means constant, 1 means index of variable
        {
            if (argument_b >= memory.size())
            {
                std::cout << "Access violation on read 0x" << std::hex << static_cast<MaxDataType>(argument_b) << std::endl;
                return true;
            }
            argument_b = memory[argument_b];
        }
        bool result = false;
        switch (instruction->op_code)
        {
        case EOpCode::IfALessB:
            result = argument_a < argument_b;
            break;
        case EOpCode::IfALessEqB:
            result = argument_a <= argument_b;
            break;
        case EOpCode::IfAEqualB:
            result = argument_a == argument_b;
            break;
        case EOpCode::IfANotEqualB:
            result = argument_a != argument_b;
            break;
        case EOpCode::IfAGreaterB:
            result = argument_a > argument_b;
            break;
        case EOpCode::IfAGreaterEqB:
            result = argument_a >= argument_b;
            break;
        default:
            std::cout << "Invalid opcode 0x" << std::hex << static_cast<MaxDataType>(instruction->op_code) << std::endl;
            return true;
        }

        if (result)
        {
            ip = instruction->new_address;
        }
        else
        {
            ip += sizeof(BinaryOperationAndAssignmentInstruction);
        }
        return false;
    }
    std::cout << "Instruction is not complete" << std::endl;
    return true;
}

inline bool executeGoto(std::vector<DataType>& memory, std::size_t& ip)
{
    if (memory.size() >= ip + sizeof(GotoInstruction))
    {
        GotoInstruction* instruction = reinterpret_cast<GotoInstruction*>(&memory[ip]);
        DataType new_address = instruction->new_address_or_label_index;
        switch (instruction->op_code)
        {
        case EOpCode::Goto:
            break;
        case EOpCode::IndirectGoto:
            if (new_address >= memory.size())
            {
                std::cout << "Access violation on read 0x" << std::hex << static_cast<MaxDataType>(new_address) << std::endl;
                return true;
            }
            new_address = memory[new_address];
            break;
        default:
            std::cout << "Invalid opcode 0x" << std::hex << static_cast<MaxDataType>(instruction->op_code) << std::endl;
            return true;
        }

        ip = new_address;
        return false;
    }
    std::cout << "Instruction is not complete" << std::endl;
    return true;
}

inline bool executeAllocate(std::vector<DataType>& memory, std::size_t& ip)
{
    if (memory.size() >= ip + sizeof(AllocateInstruction))
    {
        AllocateInstruction* instruction = reinterpret_cast<AllocateInstruction*>(&memory[ip]);
        if (instruction->variable_index >= memory.size())
        {
            std::cout << "Access violation on read 0x" << std::hex << static_cast<MaxDataType>(instruction->variable_index) << std::endl;
            return true;
        }
        std::size_t new_capacity = memory[instruction->variable_index] + memory.size();
        if (new_capacity >= std::numeric_limits<MaxDataType>::max())
        {
            std::cout << "Memory is exhausted, required size is " << std::hex << static_cast<std::size_t>(new_capacity) << std::endl;
            return true;
        }
        memory.resize(new_capacity);
        ip += sizeof(BinaryOperationAndAssignmentInstruction);
        return false;
    }
    std::cout << "Instruction is not complete" << std::endl;
    return true;
}

inline bool executeInstruction(std::vector<DataType>& memory, std::size_t& ip)
{
    if (ip >= memory.size())
    {
        std::cout << "Program completed abnormal" << std::endl;
        return true;
    }

    DataType op_code_raw = memory[ip];
    EOpCode op_code = static_cast<EOpCode>(op_code_raw);
    switch (op_code)
    {
    case EOpCode::Nop:
        return executeNop(memory, ip);
    case EOpCode::Add:
        return executeBinaryOperationAndAssignment(memory, ip);
    case EOpCode::Sub:
        return executeBinaryOperationAndAssignment(memory, ip);
    case EOpCode::Mul:
        return executeBinaryOperationAndAssignment(memory, ip);
    case EOpCode::Div:
        return executeBinaryOperationAndAssignment(memory, ip);
    case EOpCode::Mod:
        return executeBinaryOperationAndAssignment(memory, ip);
    case EOpCode::IfALessB:
        return executeIf(memory, ip);
    case EOpCode::IfALessEqB:
        return executeIf(memory, ip);
    case EOpCode::IfAEqualB:
        return executeIf(memory, ip);
    case EOpCode::IfANotEqualB:
        return executeIf(memory, ip);
    case EOpCode::IfAGreaterB:
        return executeIf(memory, ip);
    case EOpCode::IfAGreaterEqB:
        return executeIf(memory, ip);
    case EOpCode::Goto:
        return executeGoto(memory, ip);
    case EOpCode::IndirectGoto:
        return executeGoto(memory, ip);
    case EOpCode::Allocate:
        return executeAllocate(memory, ip);
    case EOpCode::Stop:
        std::cout << "Program completed successfully" << std::endl;
        return true;
    default:
        std::cout << "Invalid opcode 0x" << std::hex << static_cast<MaxDataType>(op_code_raw) << std::endl;
        return true;
    }
}
