#include "CPU.h"
#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

/**
 * Construtor da classe
 *
 * @param string instructions[]
 * @param int size
 */
CPU::CPU(string instructions[], int size, bool execute)
{
    this->auxExecute = execute;
    this->pc = 0;
    this->IMSize = size * 4;
    this->instructionMemory = new unsigned int[this->IMSize];
    this->registerBank = new int[32];
    this->dataMemory = new int[256];

    this->regDst = false;
    this->branch = false;
    this->memRead = false;
    this->memToReg = false;
    this->aluOp0 = false;
    this->aluOp1 = false;
    this->memWrite = false;
    this->aluSrc = false;
    this->regWrite = false;
    this->zero = false;
    this->branchNotEqual = false;
    this->jump = false;
    this->jumpRegister = false;
    this->multDiv = false;

    this->startArray(this->registerBank, 32);
    this->startArray(this->dataMemory, 256);
    this->hi = 0;
    this->lo = 0;
    for (int i = 0; i < size; i++)
    {
        unsigned int instructionToInt = 0;

        for (int j = 0; j < 32; j++)
        {
            instructionToInt += ((int)instructions[i][j] - 48) * pow(2, 31 - j);
        }

        this->instructionMemory[i * 4] = instructionToInt;
    }
}

/**
*Destrutor da classe
* 
* 
*/
CPU::~CPU()
{
    delete[] this->instructionMemory;
    delete[] this->registerBank;
    delete[] this->dataMemory;
}

/**
 * Irá definir os valores do sinal dos sinais de controle baseado no OPCode
 * @param int opcode
 * @return void
 */
void CPU::control(int opcode)
{
    switch (opcode)
    {
    case 0: //Tipo R
        this->regDst = true;
        this->aluSrc = false;
        this->memToReg = false;
        this->regWrite = true;
        this->memRead = false;
        this->memWrite = false;
        this->branch = false;
        this->aluOp1 = true;
        this->aluOp0 = false;
        this->jump = false;
        this->jal = false;
        this->multDiv = false;
        break;
    case 2: // j
        this->regDst = false;
        this->aluSrc = false;
        this->memToReg = false;
        this->regWrite = false;
        this->memRead = false;
        this->memWrite = false;
        this->branch = false;
        this->aluOp1 = false;
        this->aluOp0 = false;
        this->jump = true;
        this->jal = true;
        this->multDiv = false;
        break;
    case 3: //jal
        this->regWrite = true;
        this->regDst = true;
        this->memWrite = false;
        this->jump = true;
        this->jal = true;
        this->multDiv = false;
    case 4: //beq
        this->aluSrc = false;
        this->regWrite = false;
        this->memRead = false;
        this->memWrite = false;
        this->branch = true;
        this->aluOp1 = false;
        this->aluOp0 = true;
        this->jump = false;
        this->jal = false;
        this->multDiv = false;
        break;
    case 5: //bne
        this->aluSrc = false;
        this->regWrite = false;
        this->memRead = false;
        this->memWrite = false;
        this->branch = false;
        this->aluOp1 = false;
        this->aluOp0 = true;
        this->jump = false;
        this->branchNotEqual = true;
        this->jal = false;
        this->multDiv = false;
        break;
    case 8: //addi
        this->regDst = false;
        this->aluSrc = true;
        this->memToReg = false;
        this->regWrite = true;
        this->memRead = false;
        this->memWrite = false;
        this->branch = false;
        this->aluOp0 = false;
        this->aluOp1 = false;
        this->jump = false;
        this->jal = false;
        this->multDiv = false;
        break;
    case 35: //lw
        this->regDst = false;
        this->aluSrc = true;
        this->memToReg = true;
        this->regWrite = true;
        this->memRead = true;
        this->memWrite = false;
        this->branch = false;
        this->aluOp1 = false;
        this->aluOp0 = false;
        this->jump = false;
        this->jal = false;
        this->multDiv = false;
        break;
    case 43: //sw
        this->aluSrc = true;
        this->regWrite = false;
        this->memRead = false;
        this->memWrite = true;
        this->branch = false;
        this->aluOp1 = false;
        this->aluOp0 = false;
        this->jump = false;
        this->jal = false;
        this->multDiv = false;
        break;
    default:
        break;
    }
}

/**
 * Transforma um valor de 16 bits em 32 bits, duplicando o bit mais significativo
 * 
 * @return int
 */
int CPU::signExtend()
{
    int number16bits = this->instructionMemory[this->pc] & 65535;
    int mostSignificantBit = number16bits >> 15;
    int number32bits;
    if (mostSignificantBit)
    {
        number32bits = number16bits | 4294901760;
    }
    else
    {
        number32bits = number16bits;
    }

    return number32bits;
}

/**
* Função que irá realizar as operações matemáticas (ALUControl está "implementada" dentro da ALU)
* 
* @param int op1
* @param int op2
* @return int
*/
int CPU::alu(int op1, int op2)
{
    int funct = this->instructionMemory[this->pc] & 63;
    int result;
    if (!(this->aluOp1) && !(this->aluOp0)) //lw sw
    {
        result = op1 + op2;
    }
    else if (this->aluOp0) //beq
    {
        result = op1 - op2;
    }
    else //Tipo R
    {
        switch (funct)
        {
        case 0:
            result = op2 << this->getRegisterAddress(6); //sll
            this->jumpRegister = false;
            break;
        case 2:
            result = op2 >> this->getRegisterAddress(6); //srl
            this->jumpRegister = false;
            break;
        case 8: //jump
            this->jumpRegister = true;
            break;
        case 24:
        {
            this->multDiv = true;
            long int auxMult = op1 * op2;
            this->hi = auxMult >> 32;
            auxMult = auxMult << 32;
            this->lo = auxMult >> 32;
            this->jumpRegister = false;
        }
        break;

        case 32:
            result = op1 + op2; //add
            this->jumpRegister = false;
            break;
        case 34:
            result = op1 - op2; //sub
            this->jumpRegister = false;
            break;
        case 36:
            result = op1 & op2; //and
            this->jumpRegister = false;
            break;
        case 37:
            result = op1 | op2; //or
            this->jumpRegister = false;
            break;
        case 42:
            if (op1 < op2) //slt
            {
                result = 1;
            }
            else
            {
                result = 0;
            }
            this->jumpRegister = false;
            break;
        default:
            break;
        }
    }

    if (result == 0)
    {
        this->zero = true;
    }
    else
    {
        this->zero = false;
    }

    return result;
}

/**
* Função que irá executar todas as intruções armazenadas
* 
* @return void
*/
void CPU::execute()
{
    ofstream exit;
    exit.open("exit.txt");
    while (this->pc < this->IMSize)
    {
        exit.seekp(0, exit.end);
        exit << "PC: " << this->pc << endl;
        exit << endl
             << "Instrução: " << this->instructionMemory[this->pc] << endl;

        //Control
        int opcode = this->instructionMemory[this->pc] >> 26;
        this->control(opcode);

        exit << endl
             << "Sinais de Controle: " << endl
             << "RegDst:" << this->regDst << endl
             << "Branch:" << this->branch << endl
             << "MemRead:" << this->memRead << endl
             << "MemToReg:" << this->memToReg << endl
             << "ALUOp0:" << this->aluOp0 << endl
             << "ALUOp1:" << this->aluOp1 << endl
             << "MemWrite:" << this->memWrite << endl
             << "ALUSrc:" << this->aluSrc << endl
             << "RegWrite:" << this->regWrite << endl
             << "BranchNotEqual:" << this->branchNotEqual << endl
             << "Jump:" << this->jump << endl
             << "Jal:" << this->jal << endl;

        //Registers
        int readData1 = this->registerBank[this->getRegisterAddress(21)];
        int readData2 = this->registerBank[this->getRegisterAddress(16)];

        int writeRegister;
        if (this->regDst)
        {
            writeRegister = this->getRegisterAddress(11);
        }
        else
        {
            writeRegister = this->getRegisterAddress(16);
        }

        //Caminho de dados
        int extendedNumber = this->signExtend();

        //ALU e ALUControl
        int aluResult;

        if (this->aluSrc)
        {
            aluResult = this->alu(readData1, extendedNumber);
        }
        else
        {
            aluResult = this->alu(readData1, readData2);
        }
        exit << "MultDiv: " << this->multDiv << endl
             << "Zero: " << this->zero << endl
             << "JumpRegister: " << this->jumpRegister << endl;

        //Data Memory
        int address = aluResult;

        int writeDataDM = readData2;
        int readDataDM;
        if (this->memWrite)
        {
            this->dataMemory[address] = writeDataDM;
        }
        if (this->memRead)
        {
            readDataDM = this->dataMemory[address];
        }

        for (int i = 0; i < 256; i = i + 4)
        {
            exit << "Memória [" << i << "]: " << this->dataMemory[i] << endl;
        }

        exit << endl;

        //Registers
        int writeDataR;
        if (this->memToReg)
        {
            writeDataR = readDataDM;
        }
        else
        {
            writeDataR = aluResult;
        }

        if (this->regWrite && !this->jumpRegister && !this->jal && !this->multDiv)
        {
            this->registerBank[writeRegister] = writeDataR;
        }
        if (this->registerBank[0] != 0)
        {
            this->registerBank[0] = 0;
        }
        for (int i = 0; i < 32; i++)
        {
            exit << "Registrador [" << i << "]: " << this->registerBank[i] << endl;
        }

        exit << "Hi: " << this->hi << endl
             << "Lo: " << this->lo << endl
             << "-------------------------------------------------------------------------------------------"
             << endl
             << endl;

        if (this->auxExecute)
        {
            int aux;
            cout << "Escolha uma das opções abaixo para continuar a execução:" << endl;
            cout << "[ 0 ] Limpar registradores e memória" << endl;
            cout << "[ 1 ] Continuar" << endl;
            cin >> aux;
            if (aux == 0)
            {
                this->reset();
            }
        }

        //Branch
        this->pc = this->pc + 4;
        if ((this->branch && this->zero) || (this->branchNotEqual && !this->zero))
        {
            this->pc = this->pc + (extendedNumber << 2);
        }
        //jal
        if (this->jal)
        {
            this->registerBank[31] = this->pc + 4;
        }
        //jump
        if (this->jump)
        {
            int instructionJump = this->instructionMemory[this->pc - 4] & 67108863;
            instructionJump = instructionJump << 2;
            int pcJump = (this->pc) & 4026531840;
            this->pc = pcJump | instructionJump;
        }

        //jumpRegister
        if (this->jumpRegister)
        {
            this->pc = readData1;
        }
    }
    exit.close();
}

/**
* Função que irá limpar a memória e os registradores
* 
* @return void
*/
void CPU::reset()
{
    delete[] this->instructionMemory;
    delete[] this->registerBank;
    this->registerBank = new int[32];
    this->dataMemory = new int[256];
    this->startArray(this->registerBank, 32);
    this->startArray(this->dataMemory, 256);
}

/**
 * Função que inicia todas as posições do vetor com valor igual a zero
 * 
 * @param int vet
 * @param int size
 * @return int
*/
void CPU::startArray(int vet[], int size)
{
    for (int i = 0; i < size; i++)
    {
        vet[i] = 0;
    }
}

/**
 * Retorna o endereço do registrador baseado na posição que ele se encontra na instrução
 * 
 * @param int shitRight
 * @return int
*/
int CPU::getRegisterAddress(int shiftRight)
{
    int reg = this->instructionMemory[this->pc] >> shiftRight;
    reg = reg & 31;
    return reg;
}