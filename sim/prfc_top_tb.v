`timescale 1ns / 1ps

//======================================================================
// Module: prfc_top_tb
// Description: Testbench for the 'prfc_top' module.
// Verifies pRFC classifier functionality via latency and throughput tests,
// logging all detailed activity to "prfc_test.log".
//======================================================================

module prfc_top_tb();

    parameter CLK_PERIOD = 4;  // 250MHZ
    
    // DUT signals
    reg clk, rst_n;
    reg packet_valid;
    reg [31:0] src_ip_int, dst_ip_int;
    reg [15:0] src_port, dst_port;
    wire packet_ready;
    wire result_valid;
    wire [31:0] classification_result;
    reg result_ready;

    // Test Data and Logging
    reg [31:0] test_packets [0:9][0:3]; // [packet_index][field]: src_ip, dst_ip, src_port, dst_port
    integer log_file;
    reg enable_pipeline_logging; // Control for verbose pipeline state logging


    // DUT Instantiation
    prfc_top #(
        .DATA_WIDTH(32),
        .ADDR_WIDTH(16),
        .NUM_CLASSIFIERS(4),
        
        // --- Parameters for Classifier 0 (acl ruleset) ---
        .C0_PHASE0_CHUNK_NUM_0(32'd65536),
        .C0_PHASE0_CHUNK_NUM_1(32'd4096),
        .C0_PHASE0_CHUNK_NUM_2(32'd32768),
        .C0_PHASE0_CHUNK_NUM_3(32'd1),
        .C0_PHASE0_CHUNK_NUM_4(32'd65536),
        .C0_PHASE0_CHUNK_NUM_5(32'd65536),
        .C0_PHASE1_CHUNK_NUM_0(32'd106600),
        .C0_PHASE1_CHUNK_NUM_1(32'd104),
        .C0_FINALPHASE_CHUNK_NUM(32'd395512),
        .C0_PHASE0_EqIDTable_NUM_0(16'd82),
        .C0_PHASE0_EqIDTable_NUM_1(16'd50),
        .C0_PHASE0_EqIDTable_NUM_2(16'd104),
        .C0_PHASE0_EqIDTable_NUM_3(16'd1),
        .C0_PHASE0_EqIDTable_NUM_4(16'd1),
        .C0_PHASE0_EqIDTable_NUM_5(16'd26),
        .C0_PHASE1_EqIDTable_NUM_0(16'd3803),
        .C0_PHASE1_EqIDTable_NUM_1(16'd104),
        .C0_PHASE0_FACTOR_0(5'd0),
        .C0_PHASE0_FACTOR_1(5'd4),
        .C0_PHASE0_FACTOR_2(5'd1),
        .C0_PHASE0_FACTOR_3(5'd16),
        .C0_PHASE0_FACTOR_4(5'd0),
        .C0_PHASE0_FACTOR_5(5'd0),
        
        // --- Parameters for Classifier 1 (acl ruleset) ---
        .C1_PHASE0_CHUNK_NUM_0(32'd65536),
        .C1_PHASE0_CHUNK_NUM_1(32'd4096),
        .C1_PHASE0_CHUNK_NUM_2(32'd65536),
        .C1_PHASE0_CHUNK_NUM_3(32'd65536),
        .C1_PHASE0_CHUNK_NUM_4(32'd65536),
        .C1_PHASE0_CHUNK_NUM_5(32'd65536),
        .C1_PHASE1_CHUNK_NUM_0(32'd114750),
        .C1_PHASE1_CHUNK_NUM_1(32'd1819),
        .C1_FINALPHASE_CHUNK_NUM(32'd133998),
        .C1_PHASE0_EqIDTable_NUM_0(16'd85),
        .C1_PHASE0_EqIDTable_NUM_1(16'd50),
        .C1_PHASE0_EqIDTable_NUM_2(16'd17),
        .C1_PHASE0_EqIDTable_NUM_3(16'd107),
        .C1_PHASE0_EqIDTable_NUM_4(16'd1),
        .C1_PHASE0_EqIDTable_NUM_5(16'd27),
        .C1_PHASE1_EqIDTable_NUM_0(16'd971),
        .C1_PHASE1_EqIDTable_NUM_1(16'd138),
        .C1_PHASE0_FACTOR_0(5'd0),
        .C1_PHASE0_FACTOR_1(5'd4),
        .C1_PHASE0_FACTOR_2(5'd0),
        .C1_PHASE0_FACTOR_3(5'd0),
        .C1_PHASE0_FACTOR_4(5'd0),
        .C1_PHASE0_FACTOR_5(5'd0),
        
        // --- Parameters for Classifier 2 (acl ruleset) ---
        .C2_PHASE0_CHUNK_NUM_0(32'd65536),
        .C2_PHASE0_CHUNK_NUM_1(32'd65536),
        .C2_PHASE0_CHUNK_NUM_2(32'd65536),
        .C2_PHASE0_CHUNK_NUM_3(32'd16),
        .C2_PHASE0_CHUNK_NUM_4(32'd65536),
        .C2_PHASE0_CHUNK_NUM_5(32'd65536),
        .C2_PHASE1_CHUNK_NUM_0(32'd2975),
        .C2_PHASE1_CHUNK_NUM_1(32'd1300),
        .C2_FINALPHASE_CHUNK_NUM(32'd10086),
        .C2_PHASE0_EqIDTable_NUM_0(16'd7),
        .C2_PHASE0_EqIDTable_NUM_1(16'd25),
        .C2_PHASE0_EqIDTable_NUM_2(16'd100),
        .C2_PHASE0_EqIDTable_NUM_3(16'd13),
        .C2_PHASE0_EqIDTable_NUM_4(16'd1),
        .C2_PHASE0_EqIDTable_NUM_5(16'd17),
        .C2_PHASE1_EqIDTable_NUM_0(16'd82),
        .C2_PHASE1_EqIDTable_NUM_1(16'd123),
        .C2_PHASE0_FACTOR_0(5'd0),
        .C2_PHASE0_FACTOR_1(5'd0),
        .C2_PHASE0_FACTOR_2(5'd0),
        .C2_PHASE0_FACTOR_3(5'd12),
        .C2_PHASE0_FACTOR_4(5'd0),
        .C2_PHASE0_FACTOR_5(5'd0),
        
        // --- Parameters for Classifier 3 (acl ruleset) ---
        .C3_PHASE0_CHUNK_NUM_0(32'd65536),
        .C3_PHASE0_CHUNK_NUM_1(32'd65536),
        .C3_PHASE0_CHUNK_NUM_2(32'd65536),
        .C3_PHASE0_CHUNK_NUM_3(32'd65536),
        .C3_PHASE0_CHUNK_NUM_4(32'd65536),
        .C3_PHASE0_CHUNK_NUM_5(32'd65536),
        .C3_PHASE1_CHUNK_NUM_0(32'd4725),
        .C3_PHASE1_CHUNK_NUM_1(32'd864),
        .C3_FINALPHASE_CHUNK_NUM(32'd17751),
        .C3_PHASE0_EqIDTable_NUM_0(16'd7),
        .C3_PHASE0_EqIDTable_NUM_1(16'd25),
        .C3_PHASE0_EqIDTable_NUM_2(16'd9),
        .C3_PHASE0_EqIDTable_NUM_3(16'd96),
        .C3_PHASE0_EqIDTable_NUM_4(16'd1),
        .C3_PHASE0_EqIDTable_NUM_5(16'd27),
        .C3_PHASE1_EqIDTable_NUM_0(16'd183),
        .C3_PHASE1_EqIDTable_NUM_1(16'd97),
        .C3_PHASE0_FACTOR_0(5'd0),
        .C3_PHASE0_FACTOR_1(5'd0),
        .C3_PHASE0_FACTOR_2(5'd0),
        .C3_PHASE0_FACTOR_3(5'd0),
        .C3_PHASE0_FACTOR_4(5'd0),
        .C3_PHASE0_FACTOR_5(5'd0)

    ) dut (
        .clk(clk),
        .rst_n(rst_n),
        .packet_valid(packet_valid),
        .src_ip_int(src_ip_int),
        .dst_ip_int(dst_ip_int),
        .src_port(src_port),
        .dst_port(dst_port),
        .packet_ready(packet_ready),
        .result_valid(result_valid),
        .classification_result(classification_result),
        .result_ready(result_ready)
    );

    // Clock generation
    always #(CLK_PERIOD/2) clk = ~clk;

    // Pipeline State Logger (writes to log file)
    always @(posedge clk) begin
        if (rst_n && enable_pipeline_logging) begin
            if (dut.stage1_valid || dut.stage2_valid || dut.stage3_valid || dut.stage4_valid || dut.stage5_valid) begin
                $fdisplay(log_file, "[%0t] Pipeline State: S1=%b S2=%b S3=%b S4=%b S5=%b",
                        $time, dut.stage1_valid, dut.stage2_valid, dut.stage3_valid,
                        dut.stage4_valid, dut.stage5_valid);
            end

        end
    end

    //----------------------------------------------------------------
    // Test Packet Initialization
    //----------------------------------------------------------------
    // This block initializes the 'test_packets' array with 10 specific
    // packet flows at the beginning of the simulation.
    initial begin
        // Format: src_ip, dst_ip, src_port, dst_port
        test_packets[0][0] = 32'd2111292831; test_packets[0][1] = 32'd1140850687; test_packets[0][2] = 32'd0;     test_packets[0][3] = 32'd65535;
        test_packets[1][0] = 32'd2111292832; test_packets[1][1] = 32'd2973055903; test_packets[1][2] = 32'd65535; test_packets[1][3] = 32'd80;
        test_packets[2][0] = 32'd2111292815; test_packets[2][1] = 32'd2508209;    test_packets[2][2] = 32'd0;     test_packets[2][3] = 32'd80;
        test_packets[3][0] = 32'd3790848000; test_packets[3][1] = 32'd3992977408; test_packets[3][2] = 32'd0;     test_packets[3][3] = 32'd0;
        test_packets[4][0] = 32'd3087007744; test_packets[4][1] = 32'd2973073407; test_packets[4][2] = 32'd65535; test_packets[4][3] = 32'd0;
        test_packets[5][0] = 32'd2952790015; test_packets[5][1] = 32'd2973055888; test_packets[5][2] = 32'd0;     test_packets[5][3] = 32'd65535;
        test_packets[6][0] = 32'd3118012927; test_packets[6][1] = 32'd2105282709; test_packets[6][2] = 32'd0;     test_packets[6][3] = 32'd1024;
        test_packets[7][0] = 32'd2111295487; test_packets[7][1] = 32'd4294967295; test_packets[7][2] = 32'd65535; test_packets[7][3] = 32'd0;
        test_packets[8][0] = 32'd1549664255; test_packets[8][1] = 32'd2973052928; test_packets[8][2] = 32'd0;     test_packets[8][3] = 32'd137;
        test_packets[9][0] = 32'd3118013069; test_packets[9][1] = 32'd2973055886; test_packets[9][2] = 32'd0;     test_packets[9][3] = 32'd53;
    end

    //----------------------------------------------------------------
    // Main Test Sequence
    //----------------------------------------------------------------
    // This block controls the overall flow of the simulation,
    // including setup, reset, running tests, and finishing.
    initial begin
        // --- Setup ---
        log_file = $fopen("prfc_test.log", "w");
        if (log_file == 0) begin
            $display("ERROR: Log file could not be opened. Terminating simulation.");
            $finish;
        end
        
        $fdisplay(log_file, "==================================================");
        $fdisplay(log_file, " pRFC Top Level Testbench Log");
        $fdisplay(log_file, "==================================================");
        $fdisplay(log_file, "Clock period: %0d ns (Frequency: %0d MHz)", CLK_PERIOD, 1000/CLK_PERIOD);
        
        $display("=== pRFC Testbench Started. See prfc_test.log for details. ===");
        
        // Initialize signals
        clk = 0;
        rst_n = 0;
        packet_valid = 0;
        result_ready = 1;
        enable_pipeline_logging = 1'b0; // Keep it off by default

        src_ip_int = 0;
        dst_ip_int = 0;
        src_port = 0;
        dst_port = 0;
        
        // --- Reset Sequence ---
        $display("--> Applying system reset...");
        $fdisplay(log_file, "\n[%0t] Applying system reset...", $time);
        #(CLK_PERIOD*5);
        rst_n = 1;
        #(CLK_PERIOD*3);
        $display("--> Reset complete. Starting tests.");
        $fdisplay(log_file, "[%0t] Reset complete. System ready.", $time);
        $fflush(log_file);


        // --- Run Test Scenarios ---
        enable_pipeline_logging = 1'b1; // Turn on logging for tests
        test_single_packet_latency();           // Test 1: Measure the latency of a single packet through an empty pipeline.
        wait_for_pipeline_clear();
        test_pipeline_with_specific_packets();  // Test 2: Send all packets back-to-back to measure pipeline throughput.
        wait_for_pipeline_clear();
        enable_pipeline_logging = 1'b0; // Turn off logging after tests

        
        // --- Wind Down ---
        $display("\n=== All Tests Completed Successfully ===");
        $fdisplay(log_file, "\n\n=== All Tests Completed Successfully ===");
        $fdisplay(log_file, "Total simulation time: %0d ns", $time);
        $fdisplay(log_file, "==================================================");
        
        #(CLK_PERIOD*10);
        $fflush(log_file); // Final flush before closing
        $fclose(log_file);
        $finish;
    end

    //================================================================
    // TASK: test_single_packet_latency
    // Description:
    //   Sends the first test packet into an empty pipeline and measures
    //   the time until the result is available. This determines the
    //   "first packet latency" of the design.
    //================================================================
    task test_single_packet_latency();
        integer test_start_time;
        begin
            $display("\n=== Test 1: Single Packet Latency ===");
            $fdisplay(log_file, "\n\n--- Test Scenario 1: Single Packet Latency ---");

            test_start_time = $time;
            
            // Send first packet
            @(posedge clk);
            src_ip_int = test_packets[0][0];
            dst_ip_int = test_packets[0][1];
            src_port = test_packets[0][2][15:0];
            dst_port = test_packets[0][3][15:0];
            packet_valid = 1;

            $fdisplay(log_file, "[%0t] SENT Packet %0d: SrcIP=%0d, DstIP=%0d, SrcPort=%0d, DstPort=%0d", 
                    $time, 1, src_ip_int, dst_ip_int, src_port, dst_port);
            
            // De-assert valid on the next cycle
            @(posedge clk);
            packet_valid = 0;
            
            // Wait for result
            while (!result_valid) @(posedge clk);

            $fdisplay(log_file, "[%0t] *** RESULT 1 RECEIVED: 0x%08h (%0d) ***",
                    $time, classification_result, classification_result);
            $display("--> Latency Test PASSED. Latency: %0d ns (%0d cycles)",
                    $time - test_start_time, ($time - test_start_time)/CLK_PERIOD);
            $fdisplay(log_file, "Latency: %0d ns (%0d cycles)",
                    $time - test_start_time, ($time - test_start_time)/CLK_PERIOD);
            analyze_result_to_log(classification_result, 1);
            $fflush(log_file);
        end
    endtask

    //================================================================
    // TASK: wait_for_pipeline_clear (Helper Task)
    // Description:
    //   Waits until all 'valid' flags in the DUT's pipeline stages
    //   are low, indicating that no data is being processed.
    //================================================================
    task wait_for_pipeline_clear();
        begin
            // Add a small delay to ensure the last result is processed.
            repeat(5) @(posedge clk);
            
            // Wait for all pipeline stages to be invalid (empty).
            while (dut.stage1_valid || dut.stage2_valid || dut.stage3_valid || dut.stage4_valid || dut.stage5_valid) begin
                @(posedge clk);
            end
            // $display("[%0t] Pipeline cleared", $time);
            $fdisplay(log_file, "[%0t] Pipeline is clear.", $time);
            $fflush(log_file);
        end
    endtask


    //================================================================
    // TASK: test_pipeline_with_specific_packets
    // Description:
    //   Sends all 10 predefined packets back-to-back to test the
    //   pipeline's throughput and correctness under full load.
    //================================================================
    // TASK: Test pipeline throughput with all packets
    task test_pipeline_with_specific_packets;
        integer i;
        integer start_time, end_time;
        integer results_received;
        integer packet_sent_count;
        // Arrays to store results and timestamps for unified printing
        reg [31:0] received_results [0:9];
        integer    received_times [0:9];
        begin
            $display("\n=== Test 2: Pipeline Throughput (10 Packets) ===");
            $fdisplay(log_file, "\n\n--- Test Scenario 2: Pipeline Throughput ---");
            
            results_received = 0;
            packet_sent_count = 0;
            display_test_packet_list_to_log();
            start_time = $time;

            fork
                // --- SENDER PROCESS ---
                begin
                    $fdisplay(log_file, "\n--- Sending Packets ---");
                    for (i = 0; i < 10; i = i + 1) begin
                        @(posedge clk);
                        while (!packet_ready) @(posedge clk); 
                        src_ip_int = test_packets[i][0]; dst_ip_int = test_packets[i][1];
                        src_port = test_packets[i][2][15:0]; dst_port = test_packets[i][3][15:0];
                        packet_valid = 1;
                        packet_sent_count = packet_sent_count + 1;
                        $fdisplay(log_file, "[%0t] SENT Packet %0d", $time, packet_sent_count);
                        @(posedge clk);
                        packet_valid = 0;
                    end
                    $fdisplay(log_file, "[%0t] All %0d packets sent.", $time, packet_sent_count);
                end

                // --- RECEIVER PROCESS (stores results, does not print) ---
                begin
                    while (results_received < 10) begin
                        @(posedge clk);
                        if (result_valid) begin
                            received_results[results_received] = classification_result;
                            received_times[results_received] = $time;
                            results_received = results_received + 1;
                        end
                    end
                end
            join
            
            end_time = $time;
            
            // --- UNIFIED RESULT PRINTING ---
            $fdisplay(log_file, "\n--- Received Results (Unified) ---");
            for (i = 0; i < 10; i = i + 1) begin
                $fdisplay(log_file, "[%0t] *** RESULT %0d RECEIVED: 0x%08h (%0d) ***", 
                        received_times[i], i + 1, received_results[i], received_results[i]);
                analyze_result_to_log(received_results[i], i + 1);
            end

            $display("--> Throughput Test PASSED. (%0d packets processed)", results_received);

            $fflush(log_file);
        end
    endtask


    //================================================================
    // TASK: display_test_packet_list_to_log (Helper Task)
    // Description:
    //   Writes the list of all test packets to the log file.
    //================================================================
    task display_test_packet_list_to_log();
        integer i;
        begin
            $fdisplay(log_file, "\n--- Test Packet List ---");
            for (i = 0; i < 10; i = i + 1) begin
                $fdisplay(log_file, "Packet %0d: SrcIP=%0d, DstIP=%0d, SrcPort=%0d, DstPort=%0d", 
                        i+1, test_packets[i][0], test_packets[i][1], test_packets[i][2], test_packets[i][3]);
            end
            $fdisplay(log_file, "------------------------");
            $fflush(log_file); 
        end
    endtask

  

    //================================================================
    // TASK: analyze_result_to_log (Helper Task)
    // Description:
    //   Analyzes a classification result and prints a detailed
    //   interpretation to the log file.
    //================================================================
    task analyze_result_to_log(input [31:0] result, input integer packet_num);
        begin
            if (result == 32'hFFFFFFFF) begin
                $fdisplay(log_file, "      -> Analysis: NO MATCH FOUND (Default Rule). Action: DENY/Default.");
            end else if (result < 32'd65536) begin
                $fdisplay(log_file, "      -> Analysis: VALID RULE MATCH. Matched Rule ID: %0d.", result);
            end else begin
                $fdisplay(log_file, "      -> Analysis: UNEXPECTED RESULT. Value is outside expected range.");
            end
            $fflush(log_file); 
        end
    endtask


endmodule