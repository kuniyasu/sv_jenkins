`timescale 1ps/1ps


program testbench(input clk);
  `include "xmllib.h"`

  initial begin
    sv_unit svu = new();
    `SV_INIT(svu);


    `SV_TESTCASE(svu,"aaa0_class","aaaa0");

    `SV_TESTCASE(svu,"aaa1_class","aaaa1");
    `SV_TEST_SKIP(svu);

    `SV_TESTCASE(svu,"aaa2_class","aaaa2");
    `SV_TEST_ERROR(svu,"error_type","error_message", "error_log");

    `SV_TESTCASE(svu,"aaa3_class","aaaa3");
    `SV_TEST_FAILER(svu, "failer_type", "failer_message","failer_log");

    `SV_TESTCASE(svu,"aaa4_class","aaaa4");

      #300000

    $display("Simulation has finished.");
    $finish;
  end

  final begin
    svu.display();
    svu.close();
  end

endprogram


module top;
  reg clk;
  wire rst;

  //dut dut(.clk(clk), .rst(rst));

  testbench tb( .clk(clk) );

  always begin
    clk = #100 1'b0;
    clk = #100 1'b1;
  end

  initial begin
    $shm_open("test.shm");
    $shm_probe("AC");

    #10000000

    $shm_close();
    $finish;
  end

  final begin

  end

endmodule
