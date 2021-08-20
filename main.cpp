#include <fstream>
#include <iostream>
#include <sstream>
#include "CPU.h"
int sizeFile = 0;
using namespace std;

bool validFile(string file)
{
    ifstream myFile;
    myFile.open(file);
    myFile.seekg(0, myFile.end);
    string *line = new string[(myFile.tellg() / 32)];
    myFile.seekg(0, myFile.beg);
    int i = 0;
    if (myFile.is_open())
    {
        while (getline(myFile, line[i]))
        {
            if (line[i].length() != 32)
            {
                cout << "O Arquivo enviado na linha " << i + 1 << " contém menos de 32 bits." << endl;
                return false;
            }
            for (int j = 0; j < 32; j++)
            {
                if (line[i][j] != '0' && line[i][j] != '1')
                {
                    cout << "Instrução " << i << " é inválida contém um valor diferente de 0 e 1;" << endl;
                    return false;
                }
            }
            i++;
        }
        myFile.close();
    }
    if (i == 0)
    {
        cout << "O Arquivo passado não foi encontrado" << endl;
        return false;
    }
    return true;
}
/**
 * Função para leitura do arquivo de texto passado como parâmetro
 *
 * @param string file
 * @return string
 */
string *readFile(string file)
{
    ifstream myFile;
    myFile.open(file);
    myFile.seekg(0, myFile.end);
    string *line = new string[(myFile.tellg() / 32)];
    myFile.seekg(0, myFile.beg);

    int i = 0;
    if (myFile.is_open())
    {
        while (getline(myFile, line[i]))
        {
            i++;
        }
        myFile.close();
    }
    sizeFile = i;
    return line;
}

/**
 * Função para leitura do teclado
 *
 * @return void
 */
string *readKeyBoard()
{
    int instruction, i = 0;
    string *keyboard = new string[64];
    while (instruction)
    {
    newInstruction:
        cout << "Digite uma instrução MIPS: " << endl;
        cin >> keyboard[i];
        if (keyboard[i].length() != 32)
        {
            cout << "Instrução inválida" << endl;
            goto newInstruction;
        }
        for (int j = 0; j < 32; j++)
        {
            if (keyboard[i][j] != '0' && keyboard[i][j] != '1')
            {
                cout << "Instrução inválida" << endl;
                goto newInstruction;
            }
        }
        cout << "Deseja digitar mais alguma instrução? " << endl;
        cout << "[ 0 ] Não " << endl;
        cout << "[ 1 ] Sim " << endl;
        cin >> instruction;
        i++;
    }
    sizeFile = i;
    return keyboard;
}

/**
 * Função principal do trabalho
 *
 */
int main()
{
start:
    int option, execute, end;
    string file;
    cout << "Trabalho Organização de Computadores - Gabriel Fernandes Silva" << endl;
    cout << "---------------------------------------------------------------------------------" << endl;
    cout << "Todas as impressões então sendo exibidas em um arquivo txt com o nome de exit.txt" << endl;
    cout << "Escolha uma das opções abaixo" << endl;
    cout << "[ 1 ] Leitura de arquivo" << endl;
    cout << "[ 2 ] Entrada via teclado" << endl
         << endl;
    cout << "Opção: ";
    cin >> option;
    if (option == 1)
    {
        cout << "Digite o nome do arquivo com a extensão. Além disso, o arquivo deve está dentro da pasta do projeto" << endl;
        cin >> file;
    }
questionExecute:
    cout << "Escolha uma das opções abaixo para iniciar a execução" << endl;
    cout << "[ 0 ] Direta" << endl;
    cout << "[ 1 ] Passo-a-passo" << endl;
    cout << "Opção: ";
    cin >> execute;
    if (execute != 0 && execute != 1)
    {
        cout << "Opção inválida" << endl;
        goto questionExecute;
    }
    switch (option)
    {
    case 1:
    {
        if (validFile(file))
        {
            CPU *cpu = new CPU(readFile(file), sizeFile, execute);
            cpu->execute();
        }
    }
    break;
    case 2:
    {
        CPU *cpu = new CPU(readKeyBoard(), sizeFile, execute);
        cpu->execute();
    }
    break;
    default:
        break;
    }
    cout << "Deseja Finalizar a execução do programa ? " << endl;
    cout << "[ 0 ] Não" << endl;
    cout << "[ 1 ] Sim" << endl;
    cin >> end;
    switch (end)
    {
    case 0:
        cout << "Limpando a memoria dos registradores" << endl;
        sizeFile = 0;
        goto start;
        break;
    case 1:
        cout << "Programa finalizado com sucesso" << endl;
        break;
    default:
        cout << "Programa finalizado com sucesso" << endl;
        break;
    }
}