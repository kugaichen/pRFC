`timescale 1ns / 1ps

module bram_phase1 #(
    parameter CLASSIFIER_ID = 0,
    parameter PHASE1_INDEX = 0,
    parameter ADDR_WIDTH = 32,
    parameter DATA_WIDTH = 32,
    parameter DEPTH = 32768  // can be changed
)(
    input wire clka,
    input wire ena,
    input wire [ADDR_WIDTH-1:0] addra,
    output reg [DATA_WIDTH-1:0] douta
);
    // BRAM array declaration for synthesis.
    reg [DATA_WIDTH-1:0] bram_array [0:DEPTH-1];
    
    //------------------------------------------------------------------
    // Memory Initialization Block 
    //------------------------------------------------------------------
    // A nested case statement is the standard Verilog-2001 method
    // for selecting an initialization file based on parameters.
    
    initial begin
        case (CLASSIFIER_ID)
            0: case (PHASE1_INDEX)
                0: $readmemh("./data/classifier_0_phase1_0.mem", bram_array);
                1: $readmemh("./data/classifier_0_phase1_1.mem", bram_array);
                default: $readmemh("./data/classifier_0_phase1_0.mem", bram_array);
            endcase
            1: case (PHASE1_INDEX)
                0: $readmemh("./data/classifier_1_phase1_0.mem", bram_array);
                1: $readmemh("./data/classifier_1_phase1_1.mem", bram_array);
                default: $readmemh("./data/classifier_1_phase1_0.mem", bram_array);
            endcase
            2: case (PHASE1_INDEX)
                0: $readmemh("./data/classifier_2_phase1_0.mem", bram_array);
                1: $readmemh("./data/classifier_2_phase1_1.mem", bram_array);
                default: $readmemh("./data/classifier_2_phase1_0.mem", bram_array);
            endcase
            3: case (PHASE1_INDEX)
                0: $readmemh("./data/classifier_3_phase1_0.mem", bram_array);
                1: $readmemh("./data/classifier_3_phase1_1.mem", bram_array);
                default: $readmemh("./data/classifier_3_phase1_0.mem", bram_array);
            endcase
            default: $readmemh("./data/classifier_0_phase1_0.mem", bram_array);
        endcase
    end
    
    //------------------------------------------------------------------
    // BRAM Read Logic
    //------------------------------------------------------------------
    // Standard synchronous read operation.
    always @(posedge clka) begin
        if (ena) begin
            douta <= bram_array[addra];
        end
    end



endmodule