`timescale 1ns / 1ps

module bram_final #(
    parameter CLASSIFIER_ID = 0,
    parameter ADDR_WIDTH = 32,
    parameter DATA_WIDTH = 32,
    parameter DEPTH = 16384  // can be changed
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
    // Uses a case statement to select the correct memory file based
    // on the CLASSIFIER_ID parameter. 
    initial begin
        case (CLASSIFIER_ID)
            0: $readmemh("./data/classifier_0_final.mem", bram_array);
            1: $readmemh("./data/classifier_1_final.mem", bram_array);
            2: $readmemh("./data/classifier_2_final.mem", bram_array);
            3: $readmemh("./data/classifier_3_final.mem", bram_array);
            default: $readmemh("./data/classifier_0_final.mem", bram_array);
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