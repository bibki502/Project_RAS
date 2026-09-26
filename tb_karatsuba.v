`timescale 1ns/1ps

module tb_karatsuba;
    reg clk;
    reg [7:0] A, B;
    wire [15:0] OUT;

    // Подключаем модуль
    karatsuba_mult uut (
        .clk(clk),
        .A(A),
        .B(B),
        .OUT(OUT)
    );

    // Генератор тактов clk (период 10 нс)
    always #5 clk = ~clk;

    initial begin
        $dumpfile("wave.vcd");
        $dumpvars(0, tb_karatsuba);

        clk = 0;
        A = 0;
        B = 0;
        #10;

        // Подаем 13 * 11
        A = 8'd13; 
        B = 8'd11;
        
        $display("[%0t ns]  A = %d, B = %d", $time, A, B);
        #10; $display("[%0t ns] Takt 1 (OUT = %d)", $time, OUT);
        #10; $display("[%0t ns] Takt 2 (OUT = %d)", $time, OUT);
        #10; $display("[%0t ns] Takt 3 (OUT = %d) -> DONE!", $time, OUT);

        #20;
        $finish;
    end
endmodule