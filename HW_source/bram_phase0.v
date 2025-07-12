`timescale 1ns / 1ps

module bram_phase0 #(
    // ID of the parent classifier
    parameter CLASSIFIER_ID = 0,
    // Index of the phase0 dimension
    parameter PHASE0_INDEX = 0,
    parameter ADDR_WIDTH = 16,
    parameter DATA_WIDTH = 16,
    parameter DEPTH = 65536
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
    // Uses a nested case statement to select the correct memory file
    // based on the top-level parameters. This is the standard
    // approach in Verilog-2001 for this task.
    initial begin
        case (CLASSIFIER_ID)
            0: case (PHASE0_INDEX)
                0: $readmemh("./data/classifier_0_phase0_0.mem", bram_array);
                1: $readmemh("./data/classifier_0_phase0_1.mem", bram_array);
                2: $readmemh("./data/classifier_0_phase0_2.mem", bram_array);
                3: $readmemh("./data/classifier_0_phase0_3.mem", bram_array);
                4: $readmemh("./data/classifier_0_phase0_4.mem", bram_array);
                5: $readmemh("./data/classifier_0_phase0_5.mem", bram_array);
                default: $readmemh("./data/classifier_0_phase0_0.mem", bram_array);
            endcase
            1: case (PHASE0_INDEX)
                0: $readmemh("./data/classifier_1_phase0_0.mem", bram_array);
                1: $readmemh("./data/classifier_1_phase0_1.mem", bram_array);
                2: $readmemh("./data/classifier_1_phase0_2.mem", bram_array);
                3: $readmemh("./data/classifier_1_phase0_3.mem", bram_array);
                4: $readmemh("./data/classifier_1_phase0_4.mem", bram_array);
                5: $readmemh("./data/classifier_1_phase0_5.mem", bram_array);
                default: $readmemh("./data/classifier_1_phase0_0.mem", bram_array);
            endcase
            2: case (PHASE0_INDEX)
                0: $readmemh("./data/classifier_2_phase0_0.mem", bram_array);
                1: $readmemh("./data/classifier_2_phase0_1.mem", bram_array);
                2: $readmemh("./data/classifier_2_phase0_2.mem", bram_array);
                3: $readmemh("./data/classifier_2_phase0_3.mem", bram_array);
                4: $readmemh("./data/classifier_2_phase0_4.mem", bram_array);
                5: $readmemh("./data/classifier_2_phase0_5.mem", bram_array);
                default: $readmemh("./data/classifier_2_phase0_0.mem", bram_array);
            endcase
            3: case (PHASE0_INDEX)
                0: $readmemh("./data/classifier_3_phase0_0.mem", bram_array);
                1: $readmemh("./data/classifier_3_phase0_1.mem", bram_array);
                2: $readmemh("./data/classifier_3_phase0_2.mem", bram_array);
                3: $readmemh("./data/classifier_3_phase0_3.mem", bram_array);
                4: $readmemh("./data/classifier_3_phase0_4.mem", bram_array);
                5: $readmemh("./data/classifier_3_phase0_5.mem", bram_array);
                default: $readmemh("./data/classifier_3_phase0_0.mem", bram_array);
            endcase
            default: $readmemh("./data/classifier_0_phase0_0.mem", bram_array);
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