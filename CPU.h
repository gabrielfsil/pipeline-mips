#ifndef CPU_H
#define CPU_H
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

class CPU
{
private:
    //Index de instruções
    int pc;

    //Auxíliar criado para determinar o número de intruções enviadas para execução
    int IMSize;

    //Auxiliar para definir o modelo de execução da CPU
    int auxExecute;

    //Memórias de Instruções
    unsigned int *instructionMemory;

    //Banco de Registradores
    int *registerBank;

    //Registrador especial utilizado no mult e no div
    int lo;

    //Registrador especial utilizado no mult e no div
    int hi;

    //Memória de Dados
    int *dataMemory;

    /*Sinais de Controle*/

    //Sinal de controle que define se haverá desvio condicional (bne)
    bool branchNotEqual;

    //Sinal de controle que define qual registrador será escrito as informações de Write Data em Registers
    bool regDst;

    //Sinal de controle que define se haverá desvio condicional
    bool branch;

    //Sinal de controle que define se haverá leitura da memória
    bool memRead;

    //Sinal de controle que define se Write Data de Registers será preenchido com dados da memória ou o resultado da ALU
    bool memToReg;

    //Primeiro operador de ALUOP (da direita pra esquerda)
    bool aluOp0;

    //Segundo operador de ALUOP (da direita pra esquerda)
    bool aluOp1;

    //Sinal de controle que define se haverá escrita na memória
    bool memWrite;

    //Sinal de controle que define qual será o segundo operando da ALU, o registrador rt ou immediate extendido
    bool aluSrc;

    //Sinal de controle que define se haverá escrita nos registradores
    bool regWrite;

    //Sinal de controle que define se a instrução é do tipo jump
    bool jump;

    //Sinal de controle que define se a instrução é do tipo jal
    bool jal;

    //Sinal de controle que define se a instrução é do tipo mult ou do tipo Div
    bool multDiv;

    //Sinal de controle que marca se a operação de dois valores deu 0
    bool zero;

    //Sinal de controle que verifica se o campo funct da instrução equivale  ao de jr
    bool jumpRegister;

    /*Funções que emulam estruturas da CPU*/

    /**
     * Irá definir os valores do sinal dos sinais de controle baseado no OPCode
     * @param int opcode
     * @return void
     */
    void control(int opcode);

    /**
     * Transforma um valor de 16 bits em 32 bits, duplicando o bit mais significativo
     * 
     * @return int
     */
    int signExtend();

    /**
    * Função que irá realizar as operações matemáticas (ALUControl está "implementada" dentro da ALU)
    * 
    * @param int op1
    * @param int op2
    * @return int
    */
    int alu(int op1, int op2);

    /*Funções auxiliares*/

    /**
     * Retorna o endereço do registrador baseado na posição que ele se encontra na instrução
     * 
     * @param int shitRight
     * @return int
    */
    int getRegisterAddress(int shiftRight);

    /**
     * Função que inicia todas as posições do vetor com valor igual a zero
     * 
     * @param int vet
     * @param int size
     * @return int
    */
    void startArray(int vet[], int size);

public:
    /**
     * Construtor da classe
     *
     * @param string instructions[]
     * @param int size
     */
    CPU(string instructions[], int size, bool execute);

    /**
    *Destrutor da classe
    * 
    * 
    */
    ~CPU();

    /**
    * Função que irá executar todas as intruções armazenadas
    * 
    * @return void
    */
    void execute();

    /**
    * Função que irá limpar a memória e os registradores
    * 
    * @return void
    */
    void reset();
};

#endif
