#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void verilog (int N, const string& filename) {
    ofstream file (filename);

    //Проверка открытия файла
    if (file.is_open()==false) {
        cerr << "Ошибка открытия файла для записи: " << filename << endl;
        return;
    }

    file << "//N = " << N << "\n\n";
    file << "module karatsuba_mult (" << endl;
    file << "   input wire clk," << endl;
    file << "   input wire ["<< N-1 << ":0] A," << endl;
    file << "   input wire ["<<N-1<<":0] B," << endl;
    file << "   output wire ["<< 2*N-1<<":0] OUT" << endl;
    file << ");" << endl;

    //Такт 1. Вычтсление А0, А1, В0, В1 и сумм
    int n0= N/2;     //Размер младшей части A0, B0
    int n1= N-n0;   //Размер старшей части A1, B1
    int sum_len= n1+1; //Размер сумм (A1+A0) и (B1+B0) с учетом переноса

    file << "//Такт 1. Вычтсление А0, А1, В0, В1 и сумм"<< endl;
    file << "   reg ["<< n1-1<<":0] A1_r, B1_r;" << endl;
    file << "   reg ["<< n0-1<<":0] A0_r, B0_r;" << endl;
    file << "   reg ["<<sum_len-1<<":0] sumA_r, sumB_r;" << endl;


    file << "   always @(posedge clk) begin" << endl;
    file << "       A1_r <= A["<< N-1 << ":"<<n0<<"];" << endl;
    file << "       B1_r <= B["<< N-1 << ":"<<n0<<"];" << endl;
    file << "       A0_r <= A["<< n0-1 << ":" << 0 << "];" << endl;
    file << "       B0_r <= B["<< n0-1 << ":" << 0 << "];" << endl;
    file << "       sumA_r <= A["<< N-1 << ":"<<n0<<"] + A["<< n0-1 << ":" << 0 << "];" << endl;
    file << "       sumB_r <= B["<< N-1 << ":"<<n0<<"] + B["<< n0-1 << ":" << 0 << "];" << endl;
    file << "   end" << endl;

    file << "//Такт 2. Вычтсление 3-х произведений Карацубы P0, P1, P2"<< endl;
    file << "   reg ["<< 2*n1-1<<":0] P0_r;" << endl;
    file << "   reg ["<< 2*n1-1<<":0] P1_r;"<< endl;
    file << "   reg ["<< 2*sum_len-1<<":0] P2_r;" << endl;

    file << "   always @(posedge clk) begin" << endl;
    file << "        P1_r    <= A1_r * B1_r;" << endl;
    file << "        P0_r    <= A0_r * B0_r;" << endl;
    file << "        P2_r    <= sumA_r * sumB_r;" << endl;
    file << "   end" << endl;

    file << "//Такт 3. Сборка ответа"<< endl;
    file << "   assign OUT = (P1_r << " << 2*n0 << ") + ((P2_r - P1_r - P0_r) << " << n0 << ") + P0_r;\n";

    file << "endmodule" << endl;

}

int main () {
    int N;
    string filename = "verilog.v";


    cout << "Type N" << endl;
    cin >> N;

    if (N <= 0) {
        cerr << "Ошибка: N должно быть положительным числом." << endl;
        return 1;
    }

    verilog(N, filename);

    return 0;
}
