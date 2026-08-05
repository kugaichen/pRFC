
`timescale 1ns / 1ps
//======================================================================
// Module: chunk_table_manager
// Description: This module is the core controller for the pRFC lookup
//              process. It implements a state machine to manage a
//              multi-stage lookup pipeline, coordinating reads from
//              several BRAMs to calculate the final result.
//======================================================================
module chunk_table_manager #(
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
    parameter PHASE0_FACTOR_0 = 5'd2,        // phase0_chunks[0].factor
    parameter PHASE0_FACTOR_1 = 5'd1,        // phase0_chunks[1].factor
    parameter PHASE0_FACTOR_2 = 5'd2,        // phase0_chunks[2].factor
    parameter PHASE0_FACTOR_3 = 5'd1,        // phase0_chunks[3].factor
    parameter PHASE0_FACTOR_4 = 5'd0,        // phase0_chunks[4].factor
    parameter PHASE0_FACTOR_5 = 5'd0         // phase0_chunks[5].factor
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
    output reg lookup_done,
    output reg [DATA_WIDTH-1:0] lookup_result
);

    //------------------------------------------------------------------
    // State Machine Definition
    //------------------------------------------------------------------
    localparam LOOKUP_IDLE         = 4'b0000;  // Waiting for start signal
    localparam PHASE0_READ         = 4'b0001;  // Initiate parallel reads from 6 Phase0 BRAMs
    localparam PHASE0_WAIT         = 4'b0010;  // Wait one cycle for Phase0 BRAM data to be valid
    localparam PHASE1_READ         = 4'b0011;  // Initiate parallel reads from 2 Phase1 BRAMs
    localparam PHASE1_WAIT         = 4'b0100;  // Wait one cycle for Phase1 BRAM data to be valid
    localparam FINAL_READ          = 4'b0101;  // Initiate read from the Final BRAM
    localparam FINAL_WAIT          = 4'b0110;  // Wait one cycle for Final BRAM data to be valid
    localparam LOOKUP_DONE         = 4'b0111;  // Lookup is complete, hold result and done signal

    reg [3:0] current_state, next_state;

    //------------------------------------------------------------------
    // Internal Signals and Registers
    //------------------------------------------------------------------

    // Unbundled packet dimensions
    reg [15:0] packet_dims [0:5];

    // Registers to hold intermediate values between pipeline stages
    reg [31:0] value_phase0 [0:5];
    reg [31:0] key_phase1 [0:1];
    reg [31:0] value_phase1 [0:1];
    reg [31:0] key_final;
    
    // Wires for factor parameters
    wire [4:0] phase0_factor [0:5];
    

    // BRAM Interface Signal
    wire [5:0] phase0_ena;      // six
    reg [ADDR_WIDTH-1:0] phase0_addr [0:5];
    wire [15:0] phase0_dout [0:5];

    wire [1:0] phase1_ena;      // two
    reg [32-1:0] phase1_addr [0:1];
    wire [31:0] phase1_dout [0:1];

    wire final_ena;             // one
    reg [32-1:0] final_addr;
    wire [31:0] final_dout;


    // Assign parameters to factor wires for calculations
    assign phase0_factor[0] = PHASE0_FACTOR_0;
    assign phase0_factor[1] = PHASE0_FACTOR_1;
    assign phase0_factor[2] = PHASE0_FACTOR_2;
    assign phase0_factor[3] = PHASE0_FACTOR_3;
    assign phase0_factor[4] = PHASE0_FACTOR_4;
    assign phase0_factor[5] = PHASE0_FACTOR_5;

    //------------------------------------------------------------------
    // BRAM Instantiations
    //------------------------------------------------------------------
    // Phase0 BRAMs (6 instances)
    bram_phase0 #(
        .CLASSIFIER_ID(CLASSIFIER_ID),
        .PHASE0_INDEX(0),
        .ADDR_WIDTH(ADDR_WIDTH),
        .DATA_WIDTH(16),
        .DEPTH(PHASE0_CHUNK_NUM_0)
    ) phase0_0_inst (
        .clka(clk),
        .ena(phase0_ena[0]),
        .addra(phase0_addr[0]),
        .douta(phase0_dout[0])
    );

    bram_phase0 #(
        .CLASSIFIER_ID(CLASSIFIER_ID),
        .PHASE0_INDEX(1),
        .ADDR_WIDTH(ADDR_WIDTH),
        .DATA_WIDTH(16),
        .DEPTH(PHASE0_CHUNK_NUM_1)
    ) phase0_1_inst (
        .clka(clk),
        .ena(phase0_ena[1]),
        .addra(phase0_addr[1]),
        .douta(phase0_dout[1])
    );

    bram_phase0 #(
        .CLASSIFIER_ID(CLASSIFIER_ID),
        .PHASE0_INDEX(2),
        .ADDR_WIDTH(ADDR_WIDTH),
        .DATA_WIDTH(16),
        .DEPTH(PHASE0_CHUNK_NUM_2)
    ) phase0_2_inst (
        .clka(clk),
        .ena(phase0_ena[2]),
        .addra(phase0_addr[2]),
        .douta(phase0_dout[2])
    );

    bram_phase0 #(
        .CLASSIFIER_ID(CLASSIFIER_ID),
        .PHASE0_INDEX(3),
        .ADDR_WIDTH(ADDR_WIDTH),
        .DATA_WIDTH(16),
        .DEPTH(PHASE0_CHUNK_NUM_3)
    ) phase0_3_inst (
        .clka(clk),
        .ena(phase0_ena[3]),
        .addra(phase0_addr[3]),
        .douta(phase0_dout[3])
    );

    bram_phase0 #(
        .CLASSIFIER_ID(CLASSIFIER_ID),
        .PHASE0_INDEX(4),
        .ADDR_WIDTH(ADDR_WIDTH),
        .DATA_WIDTH(16),
        .DEPTH(PHASE0_CHUNK_NUM_4)
    ) phase0_4_inst (
        .clka(clk),
        .ena(phase0_ena[4]),
        .addra(phase0_addr[4]),
        .douta(phase0_dout[4])
    );

    bram_phase0 #(
        .CLASSIFIER_ID(CLASSIFIER_ID),
        .PHASE0_INDEX(5),
        .ADDR_WIDTH(ADDR_WIDTH),
        .DATA_WIDTH(16),
        .DEPTH(PHASE0_CHUNK_NUM_5)
    ) phase0_5_inst (
        .clka(clk),
        .ena(phase0_ena[5]),
        .addra(phase0_addr[5]),
        .douta(phase0_dout[5])
    );

    // Phase1 BRAMs (2 instances)
    bram_phase1 #(
        .CLASSIFIER_ID(CLASSIFIER_ID),
        .PHASE1_INDEX(0),
        .ADDR_WIDTH(32),
        .DATA_WIDTH(32),
        .DEPTH(PHASE1_CHUNK_NUM_0)
    ) phase1_0_inst (
        .clka(clk),
        .ena(phase1_ena[0]),
        .addra(phase1_addr[0]),
        .douta(phase1_dout[0])
    );

    bram_phase1 #(
        .CLASSIFIER_ID(CLASSIFIER_ID),
        .PHASE1_INDEX(1),
        .ADDR_WIDTH(32),
        .DATA_WIDTH(32),
        .DEPTH(PHASE1_CHUNK_NUM_1)
    ) phase1_1_inst (
        .clka(clk),
        .ena(phase1_ena[1]),
        .addra(phase1_addr[1]),
        .douta(phase1_dout[1])
    );

    // Final Phase BRAM (1 instance)
    bram_final #(
        .CLASSIFIER_ID(CLASSIFIER_ID),
        .ADDR_WIDTH(32),
        .DATA_WIDTH(32),
        .DEPTH(FINALPHASE_CHUNK_NUM)
    ) final_inst (
        .clka(clk),
        .ena(final_ena),
        .addra(final_addr),
        .douta(final_dout)
    );

    //------------------------------------------------------------------
    // State Machine: Sequential Logic
    //------------------------------------------------------------------
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            current_state <= LOOKUP_IDLE;
            lookup_done <= 1'b0;
            lookup_result <= 32'h0;
        end else begin
            current_state <= next_state;
            
            case (current_state)
                LOOKUP_IDLE: begin
                    lookup_done <= 1'b0;
                    if (start_lookup) begin
                        packet_dims[0] <= packet_dims_0;
                        packet_dims[1] <= packet_dims_1;
                        packet_dims[2] <= packet_dims_2;
                        packet_dims[3] <= packet_dims_3;
                        packet_dims[4] <= packet_dims_4;
                        packet_dims[5] <= packet_dims_5;
                    end
                end
                
                PHASE0_WAIT: begin
                    // Latch the results from Phase0 BRAMs
                    value_phase0[0] <= phase0_dout[0];
                    value_phase0[1] <= phase0_dout[1];
                    value_phase0[2] <= phase0_dout[2];
                    value_phase0[3] <= phase0_dout[3];
                    value_phase0[4] <= phase0_dout[4];
                    value_phase0[5] <= phase0_dout[5];
                    
                    // // 使用从BRAM读取的数据计算key_phase1
                    // key_phase1[0] <= phase0_dout[0] * PHASE0_EqIDTable_NUM_1 * PHASE0_EqIDTable_NUM_5 + 
                    //                 phase0_dout[1] * PHASE0_EqIDTable_NUM_5 + 
                    //                 phase0_dout[5];
                    
                    // key_phase1[1] <= phase0_dout[2] * PHASE0_EqIDTable_NUM_3 * PHASE0_EqIDTable_NUM_4 + 
                    //                 phase0_dout[3] * PHASE0_EqIDTable_NUM_4 + 
                    //                 phase0_dout[4];
                end
                
                PHASE1_WAIT: begin
                    // Latch the results from Phase1 BRAMs
                    value_phase1[0] <= phase1_dout[0];
                    value_phase1[1] <= phase1_dout[1];
                    
                    // key_final <= phase1_dout[0] * PHASE1_EqIDTable_NUM_1 + phase1_dout[1];
                end
                
                FINAL_WAIT: begin
                    // Latch the final result and assert done signal
                    lookup_result <= final_dout;
                    lookup_done <= 1'b1;
                end

                LOOKUP_DONE: begin
                    // Hold the done signal and result until the next start (here)
                    if (!start_lookup) begin
                        lookup_done <= 1'b0;
                    end
                end
            endcase
        end
    end

    //------------------------------------------------------------------
    // State Machine: Combinational Logic
    //------------------------------------------------------------------
    always @(*) begin
        next_state = current_state;
        
        case (current_state)
            LOOKUP_IDLE: 
                if (start_lookup) next_state = PHASE0_READ;
            PHASE0_READ: 
                next_state = PHASE0_WAIT;    // BRAM read latency
            PHASE0_WAIT: 
                next_state = PHASE1_READ;    // Phase0 data is now available
            PHASE1_READ: 
                next_state = PHASE1_WAIT;    // BRAM read latency
            PHASE1_WAIT: 
                next_state = FINAL_READ;     // Phase1 data is now available
            FINAL_READ: 
                next_state = FINAL_WAIT;     // BRAM read latency
            FINAL_WAIT: 
                next_state = LOOKUP_DONE;    // Lookup is complete
            LOOKUP_DONE: 
                next_state = LOOKUP_IDLE;    // 
            default:
                next_state = LOOKUP_IDLE;
        endcase
    end

    // BRAM enable signals are asserted based on the current state
    assign phase0_ena = (current_state == PHASE0_READ) ? 6'b111111 : 6'b000000;
    assign phase1_ena = (current_state == PHASE1_READ) ? 2'b11 : 2'b00;
    assign final_ena = (current_state == FINAL_READ) ? 1'b1 : 1'b0;

    // BRAM address generation logic
    always @(*) begin
        // Phase0 addresses are calculated by right-shifting the packet dimension by its factor
        phase0_addr[0] = packet_dims[0] >> phase0_factor[0];
        phase0_addr[1] = packet_dims[1] >> phase0_factor[1];
        phase0_addr[2] = packet_dims[2] >> phase0_factor[2];
        phase0_addr[3] = packet_dims[3] >> phase0_factor[3];
        phase0_addr[4] = packet_dims[4] >> phase0_factor[4];
        phase0_addr[5] = packet_dims[5] >> phase0_factor[5];
        
        // Phase1 keys are calculated from the results of the Phase0 lookup
        key_phase1[0] = value_phase0[0] * PHASE0_EqIDTable_NUM_1 * PHASE0_EqIDTable_NUM_5 + 
                        value_phase0[1] * PHASE0_EqIDTable_NUM_5 + 
                        value_phase0[5];
        
        key_phase1[1] = value_phase0[2] * PHASE0_EqIDTable_NUM_3 * PHASE0_EqIDTable_NUM_4 + 
                        value_phase0[3] * PHASE0_EqIDTable_NUM_4 + 
                        value_phase0[4];

        // Phase1 addresses are the calculated keys
        phase1_addr[0] = key_phase1[0];
        phase1_addr[1] = key_phase1[1];
        
        // Final key is calculated from the results of the Phase1 lookup
        key_final = value_phase1[0] * PHASE1_EqIDTable_NUM_1 + value_phase1[1];

        // Final address is the calculated final key
        final_addr = key_final;
    end

endmodule

