`timescale 1ns / 1ps

//======================================================================
// Module: packet_parser
// Description: This module parses incoming packet fields (IPs, ports)
//              and transforms them into a 6-tuple dimension vector.
//              It uses a state machine and a valid/ready handshake
//              protocol to interface with downstream modules.
//======================================================================

module packet_parser (
    input wire clk,
    input wire rst_n,
    
    // --- Input Interface (from packet source) ---
    // Handshake: Asserts 'packet_ready' to signal it can accept data.
    input wire packet_valid,            // Indicates that input data is valid
    input wire [31:0] src_ip_int,       // fields[0] -> Source IP as a 32-bit integer
    input wire [31:0] dst_ip_int,       // fields[1] -> Destination IP as a 32-bit integer
    input wire [15:0] src_port,         // fields[2] -> Source Port
    input wire [15:0] dst_port,         // fields[3] -> Destination Port
    output reg packet_ready,            // Signals that the parser is ready for a new packet
    
    // --- Output Interface (to classifier) ---
    output reg dims_valid,             // Indicates that the output dims_bus is valid
    output reg [15:0] dim_0,           // packet.dim[0] = packet.first_src_ip (higher 16)
    output reg [15:0] dim_1,           // packet.dim[1] = packet.last_src_ip  (lower 16)
    output reg [15:0] dim_2,           // packet.dim[2] = packet.first_dst_ip (higher 16)
    output reg [15:0] dim_3,           // packet.dim[3] = packet.last_dst_ip  (lower 16)
    output reg [15:0] dim_4,           // packet.dim[4] = packet.src_port
    output reg [15:0] dim_5,           // packet.dim[5] = packet.dst_port
    input wire dims_ready
);

    //------------------------------------------------------------------
    // State Machine Definition
    //------------------------------------------------------------------
    localparam    PARSE_IDLE    = 2'b00;        // Wait for a new valid packet
    localparam    PARSE_IP      = 2'b01;        // Latch inputs and compute output dimensions
    localparam    PARSE_OUTPUT  = 2'b10;        // Hold output valid and wait for handshake
    

    reg [1:0] current_state, next_state;
    
    //------------------------------------------------------------------
    // Internal Registers
    //------------------------------------------------------------------
    // Registers to hold the latched input packet fields
    reg [31:0] src_ip_reg, dst_ip_reg;
    reg [15:0] src_port_reg, dst_port_reg;
    
    reg [15:0] first_src_ip, last_src_ip;    // packet.first_src_ip, packet.last_src_ip
    reg [15:0] first_dst_ip, last_dst_ip;    // packet.first_dst_ip, packet.last_dst_ip

    //------------------------------------------------------------------
    // State Machine: Sequential Logic
    //------------------------------------------------------------------
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            current_state <= PARSE_IDLE;
            dims_valid <= 1'b0;
            dim_0 <= 16'h0;
            dim_1 <= 16'h0;
            dim_2 <= 16'h0;
            dim_3 <= 16'h0;
            dim_4 <= 16'h0;
            dim_5 <= 16'h0;
            src_ip_reg <= 32'h0;
            dst_ip_reg <= 32'h0;
            src_port_reg <= 16'h0;
            dst_port_reg <= 16'h0;
        end else begin
            current_state <= next_state;
            
            case (current_state)
                PARSE_IDLE: begin
                    dims_valid <= 1'b0;
                    if (packet_valid) begin
                        // Latch inputs when a valid packet arrives
                        src_ip_reg <= src_ip_int;
                        dst_ip_reg <= dst_ip_int;
                        src_port_reg <= src_port;
                        dst_port_reg <= dst_port;
                    end
                end
                
                PARSE_IP: begin
                    dim_0 <= src_ip_reg[31:16];          // packet.dim[0] = packet.first_src_ip
                    dim_1 <= src_ip_reg[15:0];           // packet.dim[1] = packet.last_src_ip
                    dim_2 <= dst_ip_reg[31:16];          // packet.dim[2] = packet.first_dst_ip
                    dim_3 <= dst_ip_reg[15:0];           // packet.dim[3] = packet.last_dst_ip
                    dim_4 <= src_port_reg;               // packet.dim[4] = packet.src_port
                    dim_5 <= dst_port_reg;               // packet.dim[5] = packet.dst_port
                end
                
                PARSE_OUTPUT: begin
                if (!dims_valid) begin
                    dims_valid <= 1'b1;    
                end 
                else if (dims_ready) begin
                    dims_valid <= 1'b0;    
                end
            end
            endcase
        end
    end

    //------------------------------------------------------------------
    // State Machine: Combinational Logic & Output Generation
    //------------------------------------------------------------------
    always @(*) begin
        next_state = current_state;
        packet_ready = 1'b0;
        
        case (current_state)
            PARSE_IDLE: begin
                packet_ready = 1'b1;
                if (packet_valid) begin
                    next_state = PARSE_IP;
                end
            end
            
            PARSE_IP: begin
                // IP parsed successfully
                next_state = PARSE_OUTPUT;
            end
            
            PARSE_OUTPUT: begin
                if (dims_ready && dims_valid) begin
                    next_state = PARSE_IDLE;
                end
            end
        endcase
    end

endmodule