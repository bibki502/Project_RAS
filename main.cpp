#include <iostream>
#include <fstream>
#include <string>
#include <set>

using namespace std;

set <int> generated_modules; //множество для хранения уже сгенерированных модулей

void recursion (int n, ofstream& file) {
    
    if (generated_modules.count(n)>0) return;
    generated_modules.insert(n);
    
    //побитовая конъюнкция для базовых случаев
    if (n<=3){
        file << "module karatsuba_comb_" << n << "(" << endl;
        if (n == 1) {
            file << " input wire A," << endl;
            file << " input wire B," << endl;
            file << " output wire [1:0] OUT" << endl;
            file << ");" << endl;
            file << " assign OUT = {1'b0, A & B};" << endl;
        } else {
            file << " input wire [" << n-1 << ":0] A," << endl;
            file << " input wire [" << n-1 << ":0] B," << endl;
            file << " output wire [" << 2*n-1 << ":0] OUT" << endl;
            file << ");" << endl;
            file << " assign OUT = ";

            for (int i = 0; i < n; i++) {

                if (i > 0) file << " + ";
                file << "( (A & {" << n << "{B[" << i << "]}}) << " << i << " )";
            }
            file << ";" << endl;
        }
        file << "endmodule" << endl << endl;
        return;
        
    }

    int n0=n/2;
    int n1=n-n0;
    int sum_len=n1+1;

    recursion (n1, file);
    recursion (n0, file);
    recursion (sum_len, file);

    //сборка
    file << "module karatsuba_comb_" << n << "(" << endl;
    file << "   input wire [" << n-1 << ":0] A," << endl;
    file << "   input wire [" << n-1 << ":0] B," << endl;
    file << "   output wire [" << 2*n-1 << ":0] OUT" << endl;
    file << ");" << endl;

    file << "   wire [" << n1-1 <<":0] A1 = A ["<<n-1<< ":"<<n0<<"];" << endl;
    file << "   wire [" << n0-1 <<":0] A0 = A ["<<n0-1<< ":0];" << endl;
    file << "   wire [" << n1-1 <<":0] B1 = B ["<<n-1<< ":"<<n0<<"];" << endl;
    file << "   wire [" << n0-1 <<":0] B0 = B ["<<n0-1<< ":0];" << endl;

    file << "   wire [" << sum_len-1 <<":0] sumA = A1 + A0;" << endl;
    file << "   wire [" << sum_len-1 <<":0] sumB = B1 + B0;" << endl;

    file << "   wire [" << 2*n1-1 <<":0] P1;" << endl;
    file << "   wire [" << 2*n0-1 <<":0] P0;" << endl;
    file << "   wire [" << 2*sum_len-1 <<":0] P2;" << endl;

    file << "    karatsuba_comb_"<<n1<<" u_P1 (.A(A1), .B(B1), .OUT(P1));" << endl;
    file << "    karatsuba_comb_"<<n0<<" u_P0 (.A(A0), .B(B0), .OUT(P0));" << endl;
    file << "    karatsuba_comb_" << sum_len << " u_P2 (.A(sumA), .B(sumB), .OUT(P2));" << endl;
    
    file << " wire [" << 2*n-1 << ":0] P1_ext = P1;" << endl;
    file << " wire [" << 2*n-1 << ":0] P0_ext = P0;" << endl;
    file << " wire [" << 2*n-1 << ":0] P2_ext = P2;" << endl;
    file << " wire [" << 2*n-1 << ":0] P_mid = P2_ext - P1_ext - P0_ext;" << endl;

    file << "   assign OUT = (P1 <<" << 2*n0 <<")+((P2-P1-P0)<<"<<n0<<")+P0;" << endl;
    file << "endmodule" << endl;

}

void verilog (int N, const string& filename) {
    ofstream file (filename);

    //проверка открытия файла
    if (file.is_open()==false) {
        cerr << "Ошибка открытия файла для записи: " << filename << endl;
        return;
    }

    int n0= N/2;     //Размер младшей части A0, B0
    int n1= N-n0;   //Размер старшей части A1, B1
    int sum_len= n1+1; //Размер сумм (A1+A0) и (B1+B0) с учетом переноса

    recursion (n1, file);
    recursion (n0, file);
    recursion(sum_len, file);

    file << "//N = " << N << "\n\n";
    file << "module karatsuba_mult (" << endl;
    file << "   input wire clk," << endl;
    file << "   input wire ["<< N-1 << ":0] A," << endl;
    file << "   input wire ["<<N-1<<":0] B," << endl;
    file << "   output wire ["<< 2*N-1<<":0] OUT" << endl;
    file << ");" << endl;

    file << "//Такт 1. Вычтсление А0, А1, В0, В1 и сумм"<< endl;
    file << "   reg ["<< n1-1<<":0] A1_r, B1_r;" << endl;
    file << "   reg ["<< n0-1<<":0] A0_r, B0_r;" << endl;
    file << "   reg ["<<sum_len-1<<":0] sumA_r, sumB_r;" << endl;


    file << "   always @(posedge clk) begin" << endl;
    file << "       A1_r <= A["<< N-1 << ":"<<n0<<"];" << endl;
    file << "       B1_r <= B["<< N-1 << ":"<<n0<<"];" << endl;
    file << "       A0_r <= A["<< n0-1 << ":0];" << endl;
    file << "       B0_r <= B["<< n0-1 << ":0];" << endl;
    file << "       sumA_r <= A["<< N-1 << ":"<<n0<<"] + A["<< n0-1 << ":0];" << endl;
    file << "       sumB_r <= B["<< N-1 << ":"<<n0<<"] + B["<< n0-1 << ":0];" << endl;
    file << "   end" << endl;

    file << "//Такт 2. Вычтсление 3-х произведений Карацубы P0, P1, P2"<< endl;
    file << "   wire [" << 2*n1-1 <<":0] P1_w;" << endl;
    file << "   wire [" << 2*n0-1 <<":0] P0_w;" << endl;
    file << "   wire [" << 2*sum_len-1 <<":0] P2_w;" << endl;

    file << "    karatsuba_comb_"<<n1<<" u_P1 (.A(A1_r), .B(B1_r), .OUT(P1_w));" << endl;
    file << "    karatsuba_comb_"<<n0<<" u_P0 (.A(A0_r), .B(B0_r), .OUT(P0_w));" << endl;
    file << "    karatsuba_comb_"<<sum_len<<" u_P2 (.A(sumA_r), .B(sumB_r), .OUT(P2_w));" << endl;

    file << "   reg ["<< 2*n1-1<<":0] P1_r;" << endl;
    file << "   reg ["<< 2*n0-1<<":0] P0_r;" << endl;
    file << "   reg ["<<2*sum_len-1<<":0] P2_r;" << endl;

    file << "   always @(posedge clk) begin" << endl;
    file << "        P1_r    <= P1_w;" << endl;
    file << "        P0_r    <= P0_w;" << endl;
    file << "        P2_r    <= P2_w;" << endl;
    file << "   end" << endl;

    file << "//Такт 3. Сборка ответа"<< endl;
    file << " wire [" << 2*N-1 << ":0] P1_ext = P1_r;" << endl;
    file << " wire [" << 2*N-1 << ":0] P0_ext = P0_r;" << endl;
    file << " wire [" << 2*N-1 << ":0] P2_ext = P2_r;" << endl;
    file << " wire [" << 2*N-1 << ":0] P_mid = P2_ext - P1_ext - P0_ext;" << endl;

    file << " assign OUT = (P1_ext << " << 2*n0 << ") + (P_mid << " << n0 << ") + P0_ext;" << endl;

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

