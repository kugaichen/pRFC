`timescale 1ns / 1ps

module prfc_classifier #(
    //----------------------------------------------------------------
    // Basic Parameters
    //----------------------------------------------------------------
    parameter DATA_WIDTH = 32,
    parameter ADDR_WIDTH = 16,
    parameter CLASSIFIER_ID = 0,

    //----------------------------------------------------------------
    // Phase 0 Chunk Sizes (for 6 dimensions)
    //----------------------------------------------------------------
    parameter PHASE0_CHUNK_NUM_0 = 32'd100,  // phase0_chunks[0].item_num
    parameter PHASE0_CHUNK_NUM_1 = 32'd100,  // phase0_chunks[1].item_num
    parameter PHASE0_CHUNK_NUM_2 = 32'd100,  // phase0_chunks[2].item_num
    parameter PHASE0_CHUNK_NUM_3 = 32'd150,  // phase0_chunks[3].item_num
    parameter PHASE0_CHUNK_NUM_4 = 32'd200,  // phase0_chunks[4].item_num
    parameter PHASE0_CHUNK_NUM_5 = 32'd250,  // phase0_chunks[5].item_num

    //----------------------------------------------------------------
    // Phase 1 & Final Phase Chunk Sizes
    //----------------------------------------------------------------
    parameter PHASE1_CHUNK_NUM_0 = 32'd250,  // phase1_chunks[0].item_num
    parameter PHASE1_CHUNK_NUM_1 = 32'd250,  // phase1_chunks[1].item_num
    parameter FINALPHASE_CHUNK_NUM = 32'd250,    // phase2_chunks.item_num

    //----------------------------------------------------------------
    // Phase 0 EqIDTable Sizes (for 6 dimensions)
    //----------------------------------------------------------------
    parameter PHASE0_EqIDTable_NUM_0 = 16'd250,  // phase0_chunks[0].matched_EqIDTable->item_num
    parameter PHASE0_EqIDTable_NUM_1 = 16'd250,  // phase0_chunks[1].matched_EqIDTable->item_num
    parameter PHASE0_EqIDTable_NUM_2 = 16'd250,  // phase0_chunks[2].matched_EqIDTable->item_num
    parameter PHASE0_EqIDTable_NUM_3 = 16'd250,  // phase0_chunks[3].matched_EqIDTable->item_num
    parameter PHASE0_EqIDTable_NUM_4 = 16'd250,  // phase0_chunks[4].matched_EqIDTable->item_num
    parameter PHASE0_EqIDTable_NUM_5 = 16'd250,  // phase0_chunks[5].matched_EqIDTable->item_num

    //----------------------------------------------------------------
    // Phase 1 EqIDTable Sizes
    //----------------------------------------------------------------
    parameter PHASE1_EqIDTable_NUM_0 = 16'd250,  // phase1_chunks[0].matched_EqIDTable->item_num
    parameter PHASE1_EqIDTable_NUM_1 = 16'd250,  // phase1_chunks[1].matched_EqIDTable->item_num

    //----------------------------------------------------------------
    // Phase 0 Factor Parameters (for 6 dimensions)
    //----------------------------------------------------------------
    parameter PHASE0_FACTOR_0 = 5'd2,
    parameter PHASE0_FACTOR_1 = 5'd1,
    parameter PHASE0_FACTOR_2 = 5'd2,
    parameter PHASE0_FACTOR_3 = 5'd1,
    parameter PHASE0_FACTOR_4 = 5'd0,
    parameter PHASE0_FACTOR_5 = 5'd0
)(
    input wire clk,
    input wire rst_n,
    
    // Control Interface
    input wire start_lookup,
    input wire [15:0] packet_dims_0,
    input wire [15:0] packet_dims_1,
    input wire [15:0] packet_dims_2,
    input wire [15:0] packet_dims_3,
    input wire [15:0] packet_dims_4,
    input wire [15:0] packet_dims_5,
    output wire lookup_done,
    output wire [DATA_WIDTH-1:0] lookup_result
);

    // ChunkTable Manager Instance
    chunk_table_manager #(
        .DATA_WIDTH(DATA_WIDTH),
        .ADDR_WIDTH(ADDR_WIDTH),
        .CLASSIFIER_ID(CLASSIFIER_ID),
        // Pass down the ITEM_NUM parameters
        .PHASE0_CHUNK_NUM_0(PHASE0_CHUNK_NUM_0),
        .PHASE0_CHUNK_NUM_1(PHASE0_CHUNK_NUM_1),
        .PHASE0_CHUNK_NUM_2(PHASE0_CHUNK_NUM_2),
        .PHASE0_CHUNK_NUM_3(PHASE0_CHUNK_NUM_3),
        .PHASE0_CHUNK_NUM_4(PHASE0_CHUNK_NUM_4),
        .PHASE0_CHUNK_NUM_5(PHASE0_CHUNK_NUM_5),
        .PHASE1_CHUNK_NUM_0(PHASE1_CHUNK_NUM_0),
        .PHASE1_CHUNK_NUM_1(PHASE1_CHUNK_NUM_1),
        .FINALPHASE_CHUNK_NUM(FINALPHASE_CHUNK_NUM),

        .PHASE0_EqIDTable_NUM_0(PHASE0_EqIDTable_NUM_0),
        .PHASE0_EqIDTable_NUM_1(PHASE0_EqIDTable_NUM_1),
        .PHASE0_EqIDTable_NUM_2(PHASE0_EqIDTable_NUM_2),
        .PHASE0_EqIDTable_NUM_3(PHASE0_EqIDTable_NUM_3),
        .PHASE0_EqIDTable_NUM_4(PHASE0_EqIDTable_NUM_4),
        .PHASE0_EqIDTable_NUM_5(PHASE0_EqIDTable_NUM_5),
        .PHASE1_EqIDTable_NUM_0(PHASE1_EqIDTable_NUM_0),
        .PHASE1_EqIDTable_NUM_1(PHASE1_EqIDTable_NUM_1),

        .PHASE0_FACTOR_0(PHASE0_FACTOR_0),
        .PHASE0_FACTOR_1(PHASE0_FACTOR_1),
        .PHASE0_FACTOR_2(PHASE0_FACTOR_2),
        .PHASE0_FACTOR_3(PHASE0_FACTOR_3),
        .PHASE0_FACTOR_4(PHASE0_FACTOR_4),
        .PHASE0_FACTOR_5(PHASE0_FACTOR_5)
    ) chunk_manager_inst (
        .clk(clk),
        .rst_n(rst_n),
        .start_lookup(start_lookup),
        .packet_dims_0(packet_dims_0),
        .packet_dims_1(packet_dims_1),
        .packet_dims_2(packet_dims_2),
        .packet_dims_3(packet_dims_3),
        .packet_dims_4(packet_dims_4),
        .packet_dims_5(packet_dims_5),
        .lookup_done(lookup_done),
        .lookup_result(lookup_result)
    );

endmodule