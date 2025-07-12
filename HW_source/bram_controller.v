
`timescale 1ns / 1ps

module bram_controller #(
    parameter CLASSIFIER_ID = 0
)(
    input wire clk,
    input wire rst_n,
    
    // Phase0 BRAM接口 (6个BRAM，每个65536x16bit)
    // ADDR_WIDTH for phase0 is 16, DATA_WIDTH for phase0 is 16
    input wire [5:0] phase0_ena,
    input wire [15:0] phase0_addr [0:5], // Arrayed input address
    output wire [15:0] phase0_dout [0:5], // Arrayed output data
    
    // Phase1 BRAM接口 (2个BRAM)
    // ADDR_WIDTH for phase1 is 32, DATA_WIDTH for phase1 is 32
    input wire [1:0] phase1_ena,
    input wire [31:0] phase1_addr [0:1], // Arrayed input address
    output wire [31:0] phase1_dout [0:1], // Arrayed output data
    
    // Final BRAM接口 (1个BRAM)
    input wire final_ena,
    input wire [31:0] final_addr,
    output wire [31:0] final_dout
);

    // Phase0 BRAM实例 (6个)
    genvar i;
    generate
        for (i = 0; i < 6; i = i + 1) begin : phase0_brams
            bram_phase0 #(
                .CLASSIFIER_ID(CLASSIFIER_ID),
                .PHASE0_INDEX(i),
                .DEPTH()
            ) phase0_inst (
                .clka(clk),
                .ena(phase0_ena[i]),
                .addra(i == 0 ? phase0_addr_0 : i == 1 ? phase0_addr_1 : 
                       i == 2 ? phase0_addr_2 : i == 3 ? phase0_addr_3 :
                       i == 4 ? phase0_addr_4 : phase0_addr_5),
                .douta(i == 0 ? phase0_dout_0 : i == 1 ? phase0_dout_1 :
                       i == 2 ? phase0_dout_2 : i == 3 ? phase0_dout_3 :
                       i == 4 ? phase0_dout_4 : phase0_dout_5)
            );
        end
    endgenerate

    // Phase1 BRAM实例 (2个)
    bram_phase1 #(
        .CLASSIFIER_ID(CLASSIFIER_ID), 
        .PHASE1_INDEX(0)
        // .ADDR_WIDTH(16), // Default is 16
        // .DATA_WIDTH(32)  // Default is 32
    ) 
    phase1_0_inst (.clka(clk), .ena(phase1_ena[0]), .addra(phase1_addr_0), .douta(phase1_dout_0)); // addra now connects [15:0] to [15:0]
    
    bram_phase1 #(
        .CLASSIFIER_ID(CLASSIFIER_ID), 
        .PHASE1_INDEX(1)
        // .ADDR_WIDTH(16), // Default is 16
        // .DATA_WIDTH(32)  // Default is 32
    ) 
    phase1_1_inst (.clka(clk), .ena(phase1_ena[1]), .addra(phase1_addr_1), .douta(phase1_dout_1)); // addra now connects [15:0] to [15:0]

    // Final BRAM实例 (1个)
    bram_final #(
        .CLASSIFIER_ID(CLASSIFIER_ID)
        // .ADDR_WIDTH(16), // Default is 16
        // .DATA_WIDTH(32)  // Default is 32
    ) 
    final_inst (.clka(clk), .ena(final_ena), .addra(final_addr), .douta(final_dout)); // addra now connects [15:0] to [15:0]

endmodule