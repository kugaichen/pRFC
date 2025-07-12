`timescale 1ns / 1ps

module prfc_top #(
    parameter DATA_WIDTH = 32,
    parameter ADDR_WIDTH = 16,
    parameter NUM_CLASSIFIERS = 4,
    
    // Classifier 0 parameters
    parameter C0_PHASE0_CHUNK_NUM_0 = 32'd65536,
    parameter C0_PHASE0_CHUNK_NUM_1 = 32'd4096,
    parameter C0_PHASE0_CHUNK_NUM_2 = 32'd32768,
    parameter C0_PHASE0_CHUNK_NUM_3 = 32'd1,
    parameter C0_PHASE0_CHUNK_NUM_4 = 32'd65536,
    parameter C0_PHASE0_CHUNK_NUM_5 = 32'd65536,
    parameter C0_PHASE1_CHUNK_NUM_0 = 32'd106600,
    parameter C0_PHASE1_CHUNK_NUM_1 = 32'd104,
    parameter C0_FINALPHASE_CHUNK_NUM = 32'd395512,

    parameter C0_PHASE0_EqIDTable_NUM_0 = 16'd82,
    parameter C0_PHASE0_EqIDTable_NUM_1 = 16'd50,
    parameter C0_PHASE0_EqIDTable_NUM_2 = 16'd104,
    parameter C0_PHASE0_EqIDTable_NUM_3 = 16'd1,
    parameter C0_PHASE0_EqIDTable_NUM_4 = 16'd1,
    parameter C0_PHASE0_EqIDTable_NUM_5 = 16'd26,
    parameter C0_PHASE1_EqIDTable_NUM_0 = 16'd3803,
    parameter C0_PHASE1_EqIDTable_NUM_1 = 16'd104,

    parameter C0_PHASE0_FACTOR_0 = 5'd0,
    parameter C0_PHASE0_FACTOR_1 = 5'd4,
    parameter C0_PHASE0_FACTOR_2 = 5'd1,
    parameter C0_PHASE0_FACTOR_3 = 5'd16,
    parameter C0_PHASE0_FACTOR_4 = 5'd0,
    parameter C0_PHASE0_FACTOR_5 = 5'd0,
    
    // Classifier 1 parameters
    parameter C1_PHASE0_CHUNK_NUM_0 = 32'd65536,
    parameter C1_PHASE0_CHUNK_NUM_1 = 32'd4096,
    parameter C1_PHASE0_CHUNK_NUM_2 = 32'd65536,
    parameter C1_PHASE0_CHUNK_NUM_3 = 32'd65536,
    parameter C1_PHASE0_CHUNK_NUM_4 = 32'd65536,
    parameter C1_PHASE0_CHUNK_NUM_5 = 32'd65536,
    parameter C1_PHASE1_CHUNK_NUM_0 = 32'd114750,
    parameter C1_PHASE1_CHUNK_NUM_1 = 32'd1819,
    parameter C1_FINALPHASE_CHUNK_NUM = 32'd133998,

    parameter C1_PHASE0_EqIDTable_NUM_0 = 16'd85,
    parameter C1_PHASE0_EqIDTable_NUM_1 = 16'd50,
    parameter C1_PHASE0_EqIDTable_NUM_2 = 16'd17,
    parameter C1_PHASE0_EqIDTable_NUM_3 = 16'd107,
    parameter C1_PHASE0_EqIDTable_NUM_4 = 16'd1,
    parameter C1_PHASE0_EqIDTable_NUM_5 = 16'd27,
    parameter C1_PHASE1_EqIDTable_NUM_0 = 16'd971,
    parameter C1_PHASE1_EqIDTable_NUM_1 = 16'd138,

    parameter C1_PHASE0_FACTOR_0 = 5'd0,
    parameter C1_PHASE0_FACTOR_1 = 5'd4,
    parameter C1_PHASE0_FACTOR_2 = 5'd0,
    parameter C1_PHASE0_FACTOR_3 = 5'd0,
    parameter C1_PHASE0_FACTOR_4 = 5'd0,
    parameter C1_PHASE0_FACTOR_5 = 5'd0,
    
    // Classifier 2 parameters
    parameter C2_PHASE0_CHUNK_NUM_0 = 32'd65536,
    parameter C2_PHASE0_CHUNK_NUM_1 = 32'd65536,
    parameter C2_PHASE0_CHUNK_NUM_2 = 32'd65536,
    parameter C2_PHASE0_CHUNK_NUM_3 = 32'd16,
    parameter C2_PHASE0_CHUNK_NUM_4 = 32'd65536,
    parameter C2_PHASE0_CHUNK_NUM_5 = 32'd65536,
    parameter C2_PHASE1_CHUNK_NUM_0 = 32'd2975,
    parameter C2_PHASE1_CHUNK_NUM_1 = 32'd1300,
    parameter C2_FINALPHASE_CHUNK_NUM = 32'd10086,

    parameter C2_PHASE0_EqIDTable_NUM_0 = 16'd7,
    parameter C2_PHASE0_EqIDTable_NUM_1 = 16'd25,
    parameter C2_PHASE0_EqIDTable_NUM_2 = 16'd100,
    parameter C2_PHASE0_EqIDTable_NUM_3 = 16'd13,
    parameter C2_PHASE0_EqIDTable_NUM_4 = 16'd1,
    parameter C2_PHASE0_EqIDTable_NUM_5 = 16'd17,
    parameter C2_PHASE1_EqIDTable_NUM_0 = 16'd82,
    parameter C2_PHASE1_EqIDTable_NUM_1 = 16'd123,

    parameter C2_PHASE0_FACTOR_0 = 5'd0,
    parameter C2_PHASE0_FACTOR_1 = 5'd0,
    parameter C2_PHASE0_FACTOR_2 = 5'd0,
    parameter C2_PHASE0_FACTOR_3 = 5'd12,
    parameter C2_PHASE0_FACTOR_4 = 5'd0,
    parameter C2_PHASE0_FACTOR_5 = 5'd0,
    
    // Classifier 3 parameters
    parameter C3_PHASE0_CHUNK_NUM_0 = 32'd65536,
    parameter C3_PHASE0_CHUNK_NUM_1 = 32'd65536,
    parameter C3_PHASE0_CHUNK_NUM_2 = 32'd65536,
    parameter C3_PHASE0_CHUNK_NUM_3 = 32'd65536,
    parameter C3_PHASE0_CHUNK_NUM_4 = 32'd65536,
    parameter C3_PHASE0_CHUNK_NUM_5 = 32'd65536,
    parameter C3_PHASE1_CHUNK_NUM_0 = 32'd4725,
    parameter C3_PHASE1_CHUNK_NUM_1 = 32'd864,
    parameter C3_FINALPHASE_CHUNK_NUM = 32'd17751,

    parameter C3_PHASE0_EqIDTable_NUM_0 = 16'd7,
    parameter C3_PHASE0_EqIDTable_NUM_1 = 16'd25,
    parameter C3_PHASE0_EqIDTable_NUM_2 = 16'd9,
    parameter C3_PHASE0_EqIDTable_NUM_3 = 16'd96,
    parameter C3_PHASE0_EqIDTable_NUM_4 = 16'd1,
    parameter C3_PHASE0_EqIDTable_NUM_5 = 16'd27,
    parameter C3_PHASE1_EqIDTable_NUM_0 = 16'd183,
    parameter C3_PHASE1_EqIDTable_NUM_1 = 16'd97,

    parameter C3_PHASE0_FACTOR_0 = 5'd0,
    parameter C3_PHASE0_FACTOR_1 = 5'd0,
    parameter C3_PHASE0_FACTOR_2 = 5'd0,
    parameter C3_PHASE0_FACTOR_3 = 5'd0,
    parameter C3_PHASE0_FACTOR_4 = 5'd0,
    parameter C3_PHASE0_FACTOR_5 = 5'd0
)(
    input wire clk,
    input wire rst_n,
    
    // Packet input interface
    input wire packet_valid,
    input wire [31:0] src_ip_int,
    input wire [31:0] dst_ip_int,
    input wire [15:0] src_port,
    input wire [15:0] dst_port,
    output wire packet_ready,
    
    // Classification result output
    output wire result_valid,
    output wire [DATA_WIDTH-1:0] classification_result,
    input wire result_ready
);

    // ===== 5-Stage Pipeline Design =====
    // Stage 1: PARSE - Packet parsing
    // Stage 2: PHASE0 - Phase0 table lookup 
    // Stage 3: PHASE1 - Phase1 table lookup
    // Stage 4: FINAL - Final table lookup
    // Stage 5: OUTPUT - Result selection and output

    // Integer variables for loops
    integer i, j;

    // Pipeline registers
    reg stage1_valid, stage2_valid, stage3_valid, stage4_valid, stage5_valid;
    
    // Stage 1: Parsed packet dimensions
    reg [15:0] stage1_dims [0:5];
    
    // Stage 2: Phase0 intermediate values
    reg [15:0] stage2_dims [0:5];
    reg [15:0] stage2_phase0_values [0:NUM_CLASSIFIERS-1][0:5];
    reg [31:0] stage2_phase1_keys [0:NUM_CLASSIFIERS-1][0:1];
    
    // Stage 3: Phase1 intermediate values  
    reg [31:0] stage3_phase1_values [0:NUM_CLASSIFIERS-1][0:1];
    reg [31:0] stage3_final_keys [0:NUM_CLASSIFIERS-1];
    
    // Stage 4: Final results from all classifiers
    reg [DATA_WIDTH-1:0] stage4_results [0:NUM_CLASSIFIERS-1];
    
    // Stage 5: Selected minimum result
    reg [DATA_WIDTH-1:0] stage5_result;

    reg phase0_data_ready;          // Triggers Stage 2 computation.
    reg phase1_bram_read_en;        // Enables Phase1 BRAM read.
    reg phase1_data_ready;          // Triggers Stage 3 computation.
    reg final_bram_read_en;         // Enables Final BRAM read.
    reg final_data_ready;           // Triggers Stage 4 result latch.
    reg stage5_result_valid;        // Triggers Stage 5 output latch.
    

    // Control signals
    wire pipeline_stall;
    wire enable_pipeline;
    
    // BRAM interface signals
    wire phase0_ena_0_0, phase0_ena_0_1, phase0_ena_0_2, phase0_ena_0_3, phase0_ena_0_4, phase0_ena_0_5;
    wire phase0_ena_1_0, phase0_ena_1_1, phase0_ena_1_2, phase0_ena_1_3, phase0_ena_1_4, phase0_ena_1_5;
    wire phase0_ena_2_0, phase0_ena_2_1, phase0_ena_2_2, phase0_ena_2_3, phase0_ena_2_4, phase0_ena_2_5;
    wire phase0_ena_3_0, phase0_ena_3_1, phase0_ena_3_2, phase0_ena_3_3, phase0_ena_3_4, phase0_ena_3_5;
    
    wire phase1_ena_0_0, phase1_ena_0_1, phase1_ena_1_0, phase1_ena_1_1;
    wire phase1_ena_2_0, phase1_ena_2_1, phase1_ena_3_0, phase1_ena_3_1;
    
    wire final_ena_0, final_ena_1, final_ena_2, final_ena_3;
    
    reg [ADDR_WIDTH-1:0] phase0_addr [0:NUM_CLASSIFIERS-1][0:5];
    reg [31:0] phase1_addr [0:NUM_CLASSIFIERS-1][0:1];
    reg [31:0] final_addr [0:NUM_CLASSIFIERS-1];
    
    wire [15:0] phase0_dout [0:NUM_CLASSIFIERS-1][0:5];
    wire [31:0] phase1_dout [0:NUM_CLASSIFIERS-1][0:1];
    wire [31:0] final_dout [0:NUM_CLASSIFIERS-1];

    // Result selection logic
    reg [DATA_WIDTH-1:0] min_result;

    // Pipeline control
    assign pipeline_stall = stage5_valid && !result_ready;
    assign enable_pipeline = !pipeline_stall;

    // ===== BRAM Address Generation =====
    always @(*) begin
        // Initialize addresses
        for (i = 0; i < NUM_CLASSIFIERS; i = i + 1) begin
            for (j = 0; j < 6; j = j + 1) begin
                phase0_addr[i][j] = {ADDR_WIDTH{1'b0}};
            end
            for (j = 0; j < 2; j = j + 1) begin
                phase1_addr[i][j] = 32'h0;
            end
            final_addr[i] = 32'h0;
        end
    
        // Phase0 address generation - using current input packet
        if (packet_valid) begin
            // Classifier 0
            phase0_addr[0][0] = src_ip_int[31:16] >> C0_PHASE0_FACTOR_0;
            phase0_addr[0][1] = src_ip_int[15:0] >> C0_PHASE0_FACTOR_1;
            phase0_addr[0][2] = dst_ip_int[31:16] >> C0_PHASE0_FACTOR_2;
            phase0_addr[0][3] = dst_ip_int[15:0] >> C0_PHASE0_FACTOR_3;
            phase0_addr[0][4] = src_port >> C0_PHASE0_FACTOR_4;
            phase0_addr[0][5] = dst_port >> C0_PHASE0_FACTOR_5;
            
            // Classifier 1
            phase0_addr[1][0] = src_ip_int[31:16] >> C1_PHASE0_FACTOR_0;
            phase0_addr[1][1] = src_ip_int[15:0] >> C1_PHASE0_FACTOR_1;
            phase0_addr[1][2] = dst_ip_int[31:16] >> C1_PHASE0_FACTOR_2;
            phase0_addr[1][3] = dst_ip_int[15:0] >> C1_PHASE0_FACTOR_3;
            phase0_addr[1][4] = src_port >> C1_PHASE0_FACTOR_4;
            phase0_addr[1][5] = dst_port >> C1_PHASE0_FACTOR_5;
            
            // Classifier 2
            phase0_addr[2][0] = src_ip_int[31:16] >> C2_PHASE0_FACTOR_0;
            phase0_addr[2][1] = src_ip_int[15:0] >> C2_PHASE0_FACTOR_1;
            phase0_addr[2][2] = dst_ip_int[31:16] >> C2_PHASE0_FACTOR_2;
            phase0_addr[2][3] = dst_ip_int[15:0] >> C2_PHASE0_FACTOR_3;
            phase0_addr[2][4] = src_port >> C2_PHASE0_FACTOR_4;
            phase0_addr[2][5] = dst_port >> C2_PHASE0_FACTOR_5;
            
            // Classifier 3
            phase0_addr[3][0] = src_ip_int[31:16] >> C3_PHASE0_FACTOR_0;
            phase0_addr[3][1] = src_ip_int[15:0] >> C3_PHASE0_FACTOR_1;
            phase0_addr[3][2] = dst_ip_int[31:16] >> C3_PHASE0_FACTOR_2;
            phase0_addr[3][3] = dst_ip_int[15:0] >> C3_PHASE0_FACTOR_3;
            phase0_addr[3][4] = src_port >> C3_PHASE0_FACTOR_4;
            phase0_addr[3][5] = dst_port >> C3_PHASE0_FACTOR_5;
        end
    
        // Phase1 address generation - using stage2_phase1_keys
        if (phase1_bram_read_en) begin
            for (i = 0; i < NUM_CLASSIFIERS; i = i + 1) begin
                phase1_addr[i][0] = stage2_phase1_keys[i][0];
                phase1_addr[i][1] = stage2_phase1_keys[i][1];
            end
        end
    
        // Final address generation - using stage3_final_keys
        if (final_bram_read_en) begin
            for (i = 0; i < NUM_CLASSIFIERS; i = i + 1) begin
                final_addr[i] = stage3_final_keys[i];
            end
        end
    end

    // ===== BRAM Control Signals =====
    // Phase0 BRAM: enabled during packet_valid for stage1_valid data preparation
    assign phase0_ena_0_0 = packet_valid && enable_pipeline;
    assign phase0_ena_0_1 = packet_valid && enable_pipeline;
    assign phase0_ena_0_2 = packet_valid && enable_pipeline;
    assign phase0_ena_0_3 = packet_valid && enable_pipeline;
    assign phase0_ena_0_4 = packet_valid && enable_pipeline;
    assign phase0_ena_0_5 = packet_valid && enable_pipeline;
    
    assign phase0_ena_1_0 = packet_valid && enable_pipeline;
    assign phase0_ena_1_1 = packet_valid && enable_pipeline;
    assign phase0_ena_1_2 = packet_valid && enable_pipeline;
    assign phase0_ena_1_3 = packet_valid && enable_pipeline;
    assign phase0_ena_1_4 = packet_valid && enable_pipeline;
    assign phase0_ena_1_5 = packet_valid && enable_pipeline;
    
    assign phase0_ena_2_0 = packet_valid && enable_pipeline;
    assign phase0_ena_2_1 = packet_valid && enable_pipeline;
    assign phase0_ena_2_2 = packet_valid && enable_pipeline;
    assign phase0_ena_2_3 = packet_valid && enable_pipeline;
    assign phase0_ena_2_4 = packet_valid && enable_pipeline;
    assign phase0_ena_2_5 = packet_valid && enable_pipeline;
    
    assign phase0_ena_3_0 = packet_valid && enable_pipeline;
    assign phase0_ena_3_1 = packet_valid && enable_pipeline;
    assign phase0_ena_3_2 = packet_valid && enable_pipeline;
    assign phase0_ena_3_3 = packet_valid && enable_pipeline;
    assign phase0_ena_3_4 = packet_valid && enable_pipeline;
    assign phase0_ena_3_5 = packet_valid && enable_pipeline;
    
    // Phase1 BRAM: enabled during stage2_valid for stage3_valid data preparation
    assign phase1_ena_0_0 = phase1_bram_read_en && enable_pipeline;
    assign phase1_ena_0_1 = phase1_bram_read_en && enable_pipeline;
    assign phase1_ena_1_0 = phase1_bram_read_en && enable_pipeline;
    assign phase1_ena_1_1 = phase1_bram_read_en && enable_pipeline;
    assign phase1_ena_2_0 = phase1_bram_read_en && enable_pipeline;
    assign phase1_ena_2_1 = phase1_bram_read_en && enable_pipeline;
    assign phase1_ena_3_0 = phase1_bram_read_en && enable_pipeline;
    assign phase1_ena_3_1 = phase1_bram_read_en && enable_pipeline;
    
    // Final BRAM: enabled during stage3_valid for stage4_valid data preparation
    assign final_ena_0 = final_bram_read_en && enable_pipeline;
    assign final_ena_1 = final_bram_read_en && enable_pipeline;
    assign final_ena_2 = final_bram_read_en && enable_pipeline;
    assign final_ena_3 = final_bram_read_en && enable_pipeline;

    // ===== Stage 1: PARSE =====
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            stage1_valid <= 1'b0;
            for (i = 0; i < 6; i = i + 1) begin
                stage1_dims[i] <= 16'h0;
            end
        end else if (enable_pipeline) begin
            stage1_valid <= packet_valid;
            if (packet_valid) begin
                // Parse packet into dimensions
                stage1_dims[0] <= src_ip_int[31:16];  // src_ip high
                stage1_dims[1] <= src_ip_int[15:0];   // src_ip low
                stage1_dims[2] <= dst_ip_int[31:16];  // dst_ip high  
                stage1_dims[3] <= dst_ip_int[15:0];   // dst_ip low
                stage1_dims[4] <= src_port;           // src_port
                stage1_dims[5] <= dst_port;           // dst_port
            end
        end
    end

    // Add this register declaration with other pipeline registers or within the module
    

    // ===== Stage 2: PHASE0 =====
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            stage2_valid <= 1'b0;
            phase0_data_ready <= 1'b0; // Initialize the new delay register
            for (i = 0; i < NUM_CLASSIFIERS; i = i + 1) begin
                for (j = 0; j < 6; j = j + 1) begin
                    stage2_phase0_values[i][j] <= 16'h0;
                end
                for (j = 0; j < 2; j = j + 1) begin
                    stage2_phase1_keys[i][j] <= 32'h0;
                end
            end
            for (i = 0; i < 6; i = i + 1) begin
                stage2_dims[i] <= 16'h0;
            end
        end else if (enable_pipeline) begin
            // Propagate valid signal for the next stage
            stage2_valid <= stage1_valid;
            // Create a delayed version of stage1_valid
            phase0_data_ready <= stage1_valid;

            // Capture dimensions from Stage 1 when stage1_valid is high
            // These dimensions will be used by Stage 3, aligned with stage2_valid
            if (stage1_valid) begin
                stage2_dims[0] <= stage1_dims[0];
                stage2_dims[1] <= stage1_dims[1];
                stage2_dims[2] <= stage1_dims[2];
                stage2_dims[3] <= stage1_dims[3];
                stage2_dims[4] <= stage1_dims[4];
                stage2_dims[5] <= stage1_dims[5];
            end

            // Capture phase0 results and calculate phase1 keys when phase0_dout is stable
            // This is one cycle after stage1_valid was asserted, indicated by phase0_data_ready
            if (phase0_data_ready) begin
                // Classifier 0
                stage2_phase0_values[0][0] <= phase0_dout[0][0];
                stage2_phase0_values[0][1] <= phase0_dout[0][1];
                stage2_phase0_values[0][2] <= phase0_dout[0][2];
                stage2_phase0_values[0][3] <= phase0_dout[0][3];
                stage2_phase0_values[0][4] <= phase0_dout[0][4];
                stage2_phase0_values[0][5] <= phase0_dout[0][5];
                
                stage2_phase1_keys[0][0] <= phase0_dout[0][0] * C0_PHASE0_EqIDTable_NUM_1 * C0_PHASE0_EqIDTable_NUM_5 + 
                                           phase0_dout[0][1] * C0_PHASE0_EqIDTable_NUM_5 + 
                                           phase0_dout[0][5];
                stage2_phase1_keys[0][1] <= phase0_dout[0][2] * C0_PHASE0_EqIDTable_NUM_3 * C0_PHASE0_EqIDTable_NUM_4 + 
                                           phase0_dout[0][3] * C0_PHASE0_EqIDTable_NUM_4 + 
                                           phase0_dout[0][4];
                
                // Classifier 1
                stage2_phase0_values[1][0] <= phase0_dout[1][0];
                stage2_phase0_values[1][1] <= phase0_dout[1][1];
                stage2_phase0_values[1][2] <= phase0_dout[1][2];
                stage2_phase0_values[1][3] <= phase0_dout[1][3];
                stage2_phase0_values[1][4] <= phase0_dout[1][4];
                stage2_phase0_values[1][5] <= phase0_dout[1][5];

                stage2_phase1_keys[1][0] <= phase0_dout[1][0] * C1_PHASE0_EqIDTable_NUM_1 * C1_PHASE0_EqIDTable_NUM_5 + 
                                           phase0_dout[1][1] * C1_PHASE0_EqIDTable_NUM_5 + 
                                           phase0_dout[1][5];
                stage2_phase1_keys[1][1] <= phase0_dout[1][2] * C1_PHASE0_EqIDTable_NUM_3 * C1_PHASE0_EqIDTable_NUM_4 + 
                                           phase0_dout[1][3] * C1_PHASE0_EqIDTable_NUM_4 + 
                                           phase0_dout[1][4];

                // Classifier 2
                stage2_phase0_values[2][0] <= phase0_dout[2][0];
                stage2_phase0_values[2][1] <= phase0_dout[2][1];
                stage2_phase0_values[2][2] <= phase0_dout[2][2];
                stage2_phase0_values[2][3] <= phase0_dout[2][3];
                stage2_phase0_values[2][4] <= phase0_dout[2][4];
                stage2_phase0_values[2][5] <= phase0_dout[2][5];

                stage2_phase1_keys[2][0] <= phase0_dout[2][0] * C2_PHASE0_EqIDTable_NUM_1 * C2_PHASE0_EqIDTable_NUM_5 + 
                                           phase0_dout[2][1] * C2_PHASE0_EqIDTable_NUM_5 + 
                                           phase0_dout[2][5];
                stage2_phase1_keys[2][1] <= phase0_dout[2][2] * C2_PHASE0_EqIDTable_NUM_3 * C2_PHASE0_EqIDTable_NUM_4 + 
                                           phase0_dout[2][3] * C2_PHASE0_EqIDTable_NUM_4 + 
                                           phase0_dout[2][4];

                // Classifier 3
                stage2_phase0_values[3][0] <= phase0_dout[3][0];
                stage2_phase0_values[3][1] <= phase0_dout[3][1];
                stage2_phase0_values[3][2] <= phase0_dout[3][2];
                stage2_phase0_values[3][3] <= phase0_dout[3][3];
                stage2_phase0_values[3][4] <= phase0_dout[3][4];
                stage2_phase0_values[3][5] <= phase0_dout[3][5];

                stage2_phase1_keys[3][0] <= phase0_dout[3][0] * C3_PHASE0_EqIDTable_NUM_1 * C3_PHASE0_EqIDTable_NUM_5 + 
                                           phase0_dout[3][1] * C3_PHASE0_EqIDTable_NUM_5 + 
                                           phase0_dout[3][5];
                stage2_phase1_keys[3][1] <= phase0_dout[3][2] * C3_PHASE0_EqIDTable_NUM_3 * C3_PHASE0_EqIDTable_NUM_4 + 
                                           phase0_dout[3][3] * C3_PHASE0_EqIDTable_NUM_4 + 
                                           phase0_dout[3][4];
            end
        end
    end

  
    // ===== Stage 3: PHASE1 =====
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            stage3_valid <= 1'b0;
            phase1_bram_read_en <= 1'b0; // Initialize delay register for Stage 3
            for (i = 0; i < NUM_CLASSIFIERS; i = i + 1) begin
                stage3_final_keys[i] <= 32'h0;
                for (j = 0; j < 2; j = j + 1) begin
                    stage3_phase1_values[i][j] <= 32'h0;
                end
            end
        end else if (enable_pipeline) begin
            stage3_valid <= stage2_valid;
            phase1_bram_read_en <= stage2_valid; // Create delayed version of stage2_valid
            phase1_data_ready <= stage3_valid; // Generate phase1_data_ready

            // Capture phase1 results and calculate final keys when phase1_dout is stable
            if (phase1_data_ready) begin // Use delayed valid signal
                // Classifier 0
                stage3_phase1_values[0][0] <= phase1_dout[0][0];
                stage3_phase1_values[0][1] <= phase1_dout[0][1];
                stage3_final_keys[0] <= phase1_dout[0][0] * C0_PHASE1_EqIDTable_NUM_1 + phase1_dout[0][1];
                
                // Classifier 1
                stage3_phase1_values[1][0] <= phase1_dout[1][0];
                stage3_phase1_values[1][1] <= phase1_dout[1][1];
                stage3_final_keys[1] <= phase1_dout[1][0] * C1_PHASE1_EqIDTable_NUM_1 + phase1_dout[1][1];
                
                // Classifier 2
                stage3_phase1_values[2][0] <= phase1_dout[2][0];
                stage3_phase1_values[2][1] <= phase1_dout[2][1];
                stage3_final_keys[2] <= phase1_dout[2][0] * C2_PHASE1_EqIDTable_NUM_1 + phase1_dout[2][1];
                
                // Classifier 3
                stage3_phase1_values[3][0] <= phase1_dout[3][0];
                stage3_phase1_values[3][1] <= phase1_dout[3][1];
                stage3_final_keys[3] <= phase1_dout[3][0] * C3_PHASE1_EqIDTable_NUM_1 + phase1_dout[3][1];
            end
        end
    end

    // ===== Stage 4: FINAL =====
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            stage4_valid <= 1'b0;
            final_bram_read_en <= 1'b0; // Initialize delay register for Stage 4
            final_data_ready <= 1'b0; // Initialize new delay register
            stage5_result_valid <= 1'b0; // Initialize new delay register
            for (i = 0; i < NUM_CLASSIFIERS; i = i + 1) begin
                stage4_results[i] <= 32'hFFFFFFFF;
            end
        end else if (enable_pipeline) begin
            stage4_valid <= stage3_valid;
            final_bram_read_en <= stage4_valid; // Create delayed version of stage3_valid for BRAM enable
            final_data_ready <= final_bram_read_en; // Generate final_data_ready
            stage5_result_valid <= final_data_ready; // Generate stage5_result_valid

            // Capture final results from all classifiers when final_dout is stable
            if (final_data_ready) begin // Use delayed valid signal
                stage4_results[0] <= final_dout[0];
                stage4_results[1] <= final_dout[1];
                stage4_results[2] <= final_dout[2];
                stage4_results[3] <= final_dout[3];
            end
        end
    end

    // ===== Stage 5: OUTPUT =====
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            stage5_valid <= 1'b0;
            stage5_result <= 32'hFFFFFFFF;
        end else if (enable_pipeline) begin
            if (stage5_result_valid) begin // A new result has arrived from the pipeline.
                stage5_valid <= 1'b1;           // Set the output valid flag.
                stage5_result <= min_result;    // Latch the new minimum result.
            end else if (stage5_valid && result_ready) begin // The current result is valid and has been accepted by the consumer.
                stage5_valid <= 1'b0;          // Clear the valid flag, completing the handshake.
            end
            // Otherwise, if the pipeline is stalled (stage5_valid is high but result_ready is low)
            // or if the stage is idle (stage5_valid is low), hold the current values.
        end
    end

    // ===== Result Selection Logic =====
    always @(*) begin
        min_result = 32'd65536;  // Initialize to invalid value
        
        // Find minimum valid result
        if (stage4_results[0] < min_result && stage4_results[0] != 32'hFFFFFFFF) begin
            min_result = stage4_results[0];
        end
        
        if (stage4_results[1] < min_result && stage4_results[1] != 32'hFFFFFFFF) begin
            min_result = stage4_results[1];
        end
        
        if (stage4_results[2] < min_result && stage4_results[2] != 32'hFFFFFFFF) begin
            min_result = stage4_results[2];
        end
        
        if (stage4_results[3] < min_result && stage4_results[3] != 32'hFFFFFFFF) begin
            min_result = stage4_results[3];
        end
        
        // If no valid result found, return -1
        if (min_result == 32'd65536) begin
            min_result = 32'hFFFFFFFF;  // -1, no match
        end
    end

    // ===== BRAM Instantiation =====
    // Phase0 BRAMs for Classifier 0
    bram_phase0 #(.CLASSIFIER_ID(0), .PHASE0_INDEX(0), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C0_PHASE0_CHUNK_NUM_0)) 
        phase0_0_0 (.clka(clk), .ena(phase0_ena_0_0), .addra(phase0_addr[0][0]), .douta(phase0_dout[0][0]));
    bram_phase0 #(.CLASSIFIER_ID(0), .PHASE0_INDEX(1), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C0_PHASE0_CHUNK_NUM_1)) 
        phase0_0_1 (.clka(clk), .ena(phase0_ena_0_1), .addra(phase0_addr[0][1]), .douta(phase0_dout[0][1]));
    bram_phase0 #(.CLASSIFIER_ID(0), .PHASE0_INDEX(2), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C0_PHASE0_CHUNK_NUM_2)) 
        phase0_0_2 (.clka(clk), .ena(phase0_ena_0_2), .addra(phase0_addr[0][2]), .douta(phase0_dout[0][2]));
    bram_phase0 #(.CLASSIFIER_ID(0), .PHASE0_INDEX(3), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C0_PHASE0_CHUNK_NUM_3)) 
        phase0_0_3 (.clka(clk), .ena(phase0_ena_0_3), .addra(phase0_addr[0][3]), .douta(phase0_dout[0][3]));
    bram_phase0 #(.CLASSIFIER_ID(0), .PHASE0_INDEX(4), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C0_PHASE0_CHUNK_NUM_4)) 
        phase0_0_4 (.clka(clk), .ena(phase0_ena_0_4), .addra(phase0_addr[0][4]), .douta(phase0_dout[0][4]));
    bram_phase0 #(.CLASSIFIER_ID(0), .PHASE0_INDEX(5), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C0_PHASE0_CHUNK_NUM_5)) 
        phase0_0_5 (.clka(clk), .ena(phase0_ena_0_5), .addra(phase0_addr[0][5]), .douta(phase0_dout[0][5]));

    // Phase0 BRAMs for Classifier 1
    bram_phase0 #(.CLASSIFIER_ID(1), .PHASE0_INDEX(0), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C1_PHASE0_CHUNK_NUM_0)) 
        phase0_1_0 (.clka(clk), .ena(phase0_ena_1_0), .addra(phase0_addr[1][0]), .douta(phase0_dout[1][0]));
    bram_phase0 #(.CLASSIFIER_ID(1), .PHASE0_INDEX(1), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C1_PHASE0_CHUNK_NUM_1)) 
        phase0_1_1 (.clka(clk), .ena(phase0_ena_1_1), .addra(phase0_addr[1][1]), .douta(phase0_dout[1][1]));
    bram_phase0 #(.CLASSIFIER_ID(1), .PHASE0_INDEX(2), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C1_PHASE0_CHUNK_NUM_2)) 
        phase0_1_2 (.clka(clk), .ena(phase0_ena_1_2), .addra(phase0_addr[1][2]), .douta(phase0_dout[1][2]));
    bram_phase0 #(.CLASSIFIER_ID(1), .PHASE0_INDEX(3), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C1_PHASE0_CHUNK_NUM_3)) 
        phase0_1_3 (.clka(clk), .ena(phase0_ena_1_3), .addra(phase0_addr[1][3]), .douta(phase0_dout[1][3]));
    bram_phase0 #(.CLASSIFIER_ID(1), .PHASE0_INDEX(4), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C1_PHASE0_CHUNK_NUM_4)) 
        phase0_1_4 (.clka(clk), .ena(phase0_ena_1_4), .addra(phase0_addr[1][4]), .douta(phase0_dout[1][4]));
    bram_phase0 #(.CLASSIFIER_ID(1), .PHASE0_INDEX(5), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C1_PHASE0_CHUNK_NUM_5)) 
        phase0_1_5 (.clka(clk), .ena(phase0_ena_1_5), .addra(phase0_addr[1][5]), .douta(phase0_dout[1][5]));

    // Phase0 BRAMs for Classifier 2
    bram_phase0 #(.CLASSIFIER_ID(2), .PHASE0_INDEX(0), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C2_PHASE0_CHUNK_NUM_0)) 
        phase0_2_0 (.clka(clk), .ena(phase0_ena_2_0), .addra(phase0_addr[2][0]), .douta(phase0_dout[2][0]));
    bram_phase0 #(.CLASSIFIER_ID(2), .PHASE0_INDEX(1), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C2_PHASE0_CHUNK_NUM_1)) 
        phase0_2_1 (.clka(clk), .ena(phase0_ena_2_1), .addra(phase0_addr[2][1]), .douta(phase0_dout[2][1]));
    bram_phase0 #(.CLASSIFIER_ID(2), .PHASE0_INDEX(2), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C2_PHASE0_CHUNK_NUM_2)) 
        phase0_2_2 (.clka(clk), .ena(phase0_ena_2_2), .addra(phase0_addr[2][2]), .douta(phase0_dout[2][2]));
    bram_phase0 #(.CLASSIFIER_ID(2), .PHASE0_INDEX(3), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C2_PHASE0_CHUNK_NUM_3)) 
        phase0_2_3 (.clka(clk), .ena(phase0_ena_2_3), .addra(phase0_addr[2][3]), .douta(phase0_dout[2][3]));
    bram_phase0 #(.CLASSIFIER_ID(2), .PHASE0_INDEX(4), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C2_PHASE0_CHUNK_NUM_4)) 
        phase0_2_4 (.clka(clk), .ena(phase0_ena_2_4), .addra(phase0_addr[2][4]), .douta(phase0_dout[2][4]));
    bram_phase0 #(.CLASSIFIER_ID(2), .PHASE0_INDEX(5), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C2_PHASE0_CHUNK_NUM_5)) 
        phase0_2_5 (.clka(clk), .ena(phase0_ena_2_5), .addra(phase0_addr[2][5]), .douta(phase0_dout[2][5]));

    // Phase0 BRAMs for Classifier 3
    bram_phase0 #(.CLASSIFIER_ID(3), .PHASE0_INDEX(0), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C3_PHASE0_CHUNK_NUM_0)) 
        phase0_3_0 (.clka(clk), .ena(phase0_ena_3_0), .addra(phase0_addr[3][0]), .douta(phase0_dout[3][0]));
    bram_phase0 #(.CLASSIFIER_ID(3), .PHASE0_INDEX(1), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C3_PHASE0_CHUNK_NUM_1)) 
        phase0_3_1 (.clka(clk), .ena(phase0_ena_3_1), .addra(phase0_addr[3][1]), .douta(phase0_dout[3][1]));
    bram_phase0 #(.CLASSIFIER_ID(3), .PHASE0_INDEX(2), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C3_PHASE0_CHUNK_NUM_2)) 
        phase0_3_2 (.clka(clk), .ena(phase0_ena_3_2), .addra(phase0_addr[3][2]), .douta(phase0_dout[3][2]));
    bram_phase0 #(.CLASSIFIER_ID(3), .PHASE0_INDEX(3), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C3_PHASE0_CHUNK_NUM_3)) 
        phase0_3_3 (.clka(clk), .ena(phase0_ena_3_3), .addra(phase0_addr[3][3]), .douta(phase0_dout[3][3]));
    bram_phase0 #(.CLASSIFIER_ID(3), .PHASE0_INDEX(4), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C3_PHASE0_CHUNK_NUM_4)) 
        phase0_3_4 (.clka(clk), .ena(phase0_ena_3_4), .addra(phase0_addr[3][4]), .douta(phase0_dout[3][4]));
    bram_phase0 #(.CLASSIFIER_ID(3), .PHASE0_INDEX(5), .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(16), .DEPTH(C3_PHASE0_CHUNK_NUM_5)) 
        phase0_3_5 (.clka(clk), .ena(phase0_ena_3_5), .addra(phase0_addr[3][5]), .douta(phase0_dout[3][5]));

    // Phase1 BRAMs
    bram_phase1 #(.CLASSIFIER_ID(0), .PHASE1_INDEX(0), .ADDR_WIDTH(32), .DATA_WIDTH(32), .DEPTH(C0_PHASE1_CHUNK_NUM_0)) 
        phase1_0_0 (.clka(clk), .ena(phase1_ena_0_0), .addra(phase1_addr[0][0]), .douta(phase1_dout[0][0]));
    bram_phase1 #(.CLASSIFIER_ID(0), .PHASE1_INDEX(1), .ADDR_WIDTH(32), .DATA_WIDTH(32), .DEPTH(C0_PHASE1_CHUNK_NUM_1)) 
        phase1_0_1 (.clka(clk), .ena(phase1_ena_0_1), .addra(phase1_addr[0][1]), .douta(phase1_dout[0][1]));
    
    bram_phase1 #(.CLASSIFIER_ID(1), .PHASE1_INDEX(0), .ADDR_WIDTH(32), .DATA_WIDTH(32), .DEPTH(C1_PHASE1_CHUNK_NUM_0)) 
        phase1_1_0 (.clka(clk), .ena(phase1_ena_1_0), .addra(phase1_addr[1][0]), .douta(phase1_dout[1][0]));
    bram_phase1 #(.CLASSIFIER_ID(1), .PHASE1_INDEX(1), .ADDR_WIDTH(32), .DATA_WIDTH(32), .DEPTH(C1_PHASE1_CHUNK_NUM_1)) 
        phase1_1_1 (.clka(clk), .ena(phase1_ena_1_1), .addra(phase1_addr[1][1]), .douta(phase1_dout[1][1]));
    
    bram_phase1 #(.CLASSIFIER_ID(2), .PHASE1_INDEX(0), .ADDR_WIDTH(32), .DATA_WIDTH(32), .DEPTH(C2_PHASE1_CHUNK_NUM_0)) 
        phase1_2_0 (.clka(clk), .ena(phase1_ena_2_0), .addra(phase1_addr[2][0]), .douta(phase1_dout[2][0]));
    bram_phase1 #(.CLASSIFIER_ID(2), .PHASE1_INDEX(1), .ADDR_WIDTH(32), .DATA_WIDTH(32), .DEPTH(C2_PHASE1_CHUNK_NUM_1)) 
        phase1_2_1 (.clka(clk), .ena(phase1_ena_2_1), .addra(phase1_addr[2][1]), .douta(phase1_dout[2][1]));
    
    bram_phase1 #(.CLASSIFIER_ID(3), .PHASE1_INDEX(0), .ADDR_WIDTH(32), .DATA_WIDTH(32), .DEPTH(C3_PHASE1_CHUNK_NUM_0)) 
        phase1_3_0 (.clka(clk), .ena(phase1_ena_3_0), .addra(phase1_addr[3][0]), .douta(phase1_dout[3][0]));
    bram_phase1 #(.CLASSIFIER_ID(3), .PHASE1_INDEX(1), .ADDR_WIDTH(32), .DATA_WIDTH(32), .DEPTH(C3_PHASE1_CHUNK_NUM_1)) 
        phase1_3_1 (.clka(clk), .ena(phase1_ena_3_1), .addra(phase1_addr[3][1]), .douta(phase1_dout[3][1]));

    // Final BRAMs
    bram_final #(.CLASSIFIER_ID(0), .ADDR_WIDTH(32), .DATA_WIDTH(32), .DEPTH(C0_FINALPHASE_CHUNK_NUM)) 
        final_0 (.clka(clk), .ena(final_ena_0), .addra(final_addr[0]), .douta(final_dout[0]));
    bram_final #(.CLASSIFIER_ID(1), .ADDR_WIDTH(32), .DATA_WIDTH(32), .DEPTH(C1_FINALPHASE_CHUNK_NUM)) 
        final_1 (.clka(clk), .ena(final_ena_1), .addra(final_addr[1]), .douta(final_dout[1]));
    bram_final #(.CLASSIFIER_ID(2), .ADDR_WIDTH(32), .DATA_WIDTH(32), .DEPTH(C2_FINALPHASE_CHUNK_NUM)) 
        final_2 (.clka(clk), .ena(final_ena_2), .addra(final_addr[2]), .douta(final_dout[2]));
    bram_final #(.CLASSIFIER_ID(3), .ADDR_WIDTH(32), .DATA_WIDTH(32), .DEPTH(C3_FINALPHASE_CHUNK_NUM)) 
        final_3 (.clka(clk), .ena(final_ena_3), .addra(final_addr[3]), .douta(final_dout[3]));

    // ===== Output Interface =====
    assign packet_ready = enable_pipeline;
    assign result_valid = stage5_valid;
    assign classification_result = stage5_result;

endmodule

