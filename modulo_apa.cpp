#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>

#define INF 999999999

/*
   g++ modulo_apa.cpp -o apa
   ./apa flag file1 file2

   Flags

   -t
   -f
   -ns

   Compilando o CUDA
   ./configure --with-cuda=/usr/lib/cuda --with-nvcc=/usr/bin
    make
   Rodando o CUDA
   ./cudalign  ../trimmed_bases/AE016879.1.fasta ../trimmed_bases/AE016879.1.fasta
*/

using namespace std;

void trim(string arqv1, string cropped_arqv1, string arqv2, string cropped_arqv2)
{
    // Executa o trim no arquivo 1 e insere em trimmed_bases
    string cmd = "awk 'NR==1, NR==295380' " + arqv1 + " > " + "trimmed_bases/" + cropped_arqv1;
    system(cmd.c_str());

    // Executa o trim no arquivo 2 e insere em trimmed_bases
    cmd = "awk 'NR==1, NR==295380' " + arqv2 + " > " + "trimmed_bases/" + cropped_arqv2;
    system(cmd.c_str());
}

// Fazer a modificacao para ler apenas até o final do arquivo e caso não seja possível executar o blastn, retornar algo específico
int findScore()
{
    ifstream file;
    file.open("output");
    string line;
    int score = -INF;
    while (!file.eof() && score == -INF)
    {
        getline(file, line);
        if (line.find(" Score =") != string::npos)
        {
            int pos = line.find("(");
            int pos1 = line.find(")");
            score = atoi(line.substr(pos + 1, pos1 - pos - 1).c_str());
            cout << "Best Score gerado pelo BLASTn:" << score << endl;
        }
    }
    if (score == -INF)
    {
        cout << "Não foi possível gerar um escore pelo BLASTn. Foi aplicado o escore padrão -INF." << endl;
    }
    file.close();
    return score;
}

void sendScore(int score, string arqv1, string arqv2)
{

    string cmd = "./masa-cudalign/cudalign --ram-size=500M --disk-size=2G --opening-score=" + to_string(score) + " " + arqv1 + " " + arqv2;
    system(cmd.c_str());
}

void executeModule(string arqv1, string arqv2, int score)
{
    int opening_score;
    string cmd;
    string cropped_arqv1 = arqv1.substr(arqv1.find_last_of("/") + 1);
    string cropped_arqv2 = arqv2.substr(arqv2.find_last_of("/") + 1);

    switch (score)
    {
    // Caso escore blastn da sequencia cortada para 2M
    case 1:
        trim(arqv1, cropped_arqv1, arqv2, cropped_arqv2);

        // Executa o Blastn
        cmd = "./blastn_linux -subject trimmed_bases/" + cropped_arqv1 + " -query trimmed_bases/" + cropped_arqv2 + " -gapopen 5 -gapextend 2 -penalty -3 -reward 1 -num_alignments 1 > output";
        system(cmd.c_str());

        // Encontra o escore e coloca na variável opening_score
        opening_score = findScore();

        // Envia escore para o MASA-CUDAlign
        sendScore(opening_score, arqv1, arqv2);
        break;
    // Caso escore blastn da sequencia completa
    case 2:
        // Executa o Blastn
        cmd = "./blastn_linux -subject " + arqv1 + " -query " + arqv2 + " -gapopen 5 -gapextend 2 -penalty -3 -reward 1 -num_alignments 1 > output";
        system(cmd.c_str());

        // Encontra o escore e coloca na variável opening_score
        opening_score = findScore();

        // Envia escore para o MASA-CUDAlign
        sendScore(opening_score, arqv1, arqv2);
        break;
    // Caso sem escore do blastn
    default:
        // Envia escore para o MASA-CUDAlign
        sendScore(-INF, arqv1, arqv2);

        break;
    }
}

int main(int argc, char **argv)
{
    time_t start, end;
    time(&start);
    string flag1 = argv[1], arqv1 = argv[2], arqv2 = argv[3];

    if (flag1 == "-t" || flag1 == "--trimmed")
    {
        executeModule(arqv1, arqv2, 1);
    }
    else if (flag1 == "-f" || flag1 == "--full")
    {
        executeModule(arqv1, arqv2, 2);
    }
    else
    {
        executeModule(arqv1, arqv2, 3);
    }
    time(&end);
    double time_taken = double(end - start);
    cout << "Time taken by program is : " << fixed << time_taken << setprecision(5) << " sec"; 
    return 0;
}