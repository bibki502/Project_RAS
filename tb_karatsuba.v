`timescale 1ns / 1ps

module tb_karatsuba;

    //значение N по умолчанию (если не передано при компиляции)
    parameter N = 8;

    reg clk;
    reg [N-1:0] A;
    reg [N-1:0] B;
    wire [2*N-1:0] OUT;

    integer errors = 0;
    integer i;

    karatsuba_mult uut (
        .clk(clk),
        .A(A),
        .B(B),
        .OUT(OUT)
    );

    always #5 clk = ~clk;

    //таск для подачи входов и автоматической проверки через 3 такта
    task check_pair(input [N-1:0] a_val, input [N-1:0] b_val);
        reg [2*N-1:0] expected;
        begin
            A = a_val;
            B = b_val;
            expected = a_val * b_val;

            //3 такта работы конвейера
            @(posedge clk);
            @(posedge clk);
            @(posedge clk);

            //проверка результата
            if (OUT !== expected) begin
                $display("[ERROR] A=%d, B=%d | Expected: %d, Recieved: %d", a_val, b_val, expected, OUT);
                errors = errors + 1;
            end else begin
                $display("[SUCCES]  A=%d, B=%d | OUT=%d", a_val, b_val, OUT);
            end
        end
    endtask

    initial begin
        clk = 0;
        A = 0;
        B = 0;

        $display("\n==================================================");
        $display("   START OF TESTING KARATSUBA MULTIPLIER (N = %0d)", N);
        $display("==================================================\n");

        // 1.Граничные тесты (Corner Cases)
        $display("--- 1. Corner сases ---");
        check_pair(0, 0);
        check_pair(0, {N{1'b1}}); // 0 * MAX
        check_pair(1, 1);
        check_pair({N{1'b1}}, {N{1'b1}}); // MAX * MAX

        // 2.Случайные тесты (Randomized Verification)
        $display("\n--- 2. Randomized Verification (50 pieces) ---");
        for (i = 0; i < 50; i = i + 1) begin
            check_pair($urandom, $urandom);
        end

        //Итог
        $display("\n==================================================");
        if (errors == 0)
            $display("   >>> All tests passed successfully! <<<");
        else
            $display("   >>> ERRORS FOUND: %0d <<<", errors);
        $display("==================================================\n");

        $finish;
    end

endmodule