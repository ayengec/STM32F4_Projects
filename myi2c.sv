`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 06/22/2021 01:50:05 PM
// Design Name: 
// Module Name: i2c_master
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module i2c_master#(int unsigned input_clk=100000000, int unsigned freq=400000)
    (
        input logic         clk,            //  system main clock
        input logic         rst_n,          //  system reset
        input logic         enable,         //  enable i2c master transfer sequence
        input logic[6:0]    slv_addr,       //  Slave address of i2c
        input logic         RnW,            //  Read=1 or Write=0
        input logic[7:0]    data_wr,        //  write data to slave
        output logic        busy,           //  bus busy, i2c is in use
        output logic[7:0]   rd_data,        //  read data from slave
        output logic        nAck,           //  either ack error or not
        inout  logic        sda,            //  Serial data output/ack_in I2C bus
        output logic        scl             //  Serial clock output of bus

    );
    /////////// CLOCK DIVISION VARIABLES //////////
    int unsigned divider = (input_clk/freq);  //how many system clock needs for one SCL period
    int unsigned clk_counter;
    int unsigned scl_counter;                 // they are generated in scl_clock_generator ff
    logic scl_clock = 0;
    logic scl_sample_clk = 0;
    ////////////////////////////////////////////////
    ///

    logic scl_enable;
    logic sda_int;
    logic sda_enable_n;
    int unsigned bit_cnt;

    logic scl_clock_prev;
    
    logic[7:0] addr_rw;
    logic[7:0] data_tx;
    logic[7:0] data_rx;
    
    typedef enum logic[3:0] {
        READY,
        START,
        COMMAND,
        SLV_ACK1,
        WRITE,
        READ,
        SLV_ACK2,
        MSTR_ACK,
        STOP
    } state_t;

    state_t state;
   
    always_ff @(posedge(clk) or negedge(rst_n)) begin:  scl_clock_generator

        if(!rst_n) begin
            clk_counter     <= 0;
            scl_counter     <= 0;
            scl_clock       <= 0;
            scl_sample_clk  <= 0;
        end

        else if (clk_counter == (divider/16)) begin
                scl_sample_clk <= ~scl_sample_clk;
                clk_counter <= 0;
                scl_counter <= scl_counter + 1;
        end

        else if (scl_counter == 8) begin
            scl_clock_prev <= scl_clock;
            scl_counter <= 0;
            scl_clock   <= ~scl_clock;
        end

        else begin 
            clk_counter <= clk_counter + 1;
        end
        
    end: scl_clock_generator

    always_ff @(posedge(scl_clock) or negedge(rst_n)) begin: data_transaction_machine
        if(!rst_n) begin
            state <= READY;             // reset asserted
            busy <= 0;                  // Clear busy flag
            scl_enable <= 0;            // Sets SCL to HIGH
            sda_int <= 1;               // sets SDA to HIGH
            nAck <= 0;                  // clear nAck flag
            bit_cnt <= 7;               // restarts bit counter
            rd_data <= 'h0;             // read data port clear
        end


        else if (!scl_clock & scl_clock_prev) begin
            unique case(state)
                START:begin
                    if(scl_enable) begin
                        scl_enable <= 1;
                        nAck <= 1;
                    end
                end

                SLV_ACK1:begin
                    if (sda | nAck) begin
                        nAck <= 1;
                    end
                end

                READ:begin
                    data_rx[bit_cnt] <= sda;
                end

                SLV_ACK2:begin
                    if (sda | nAck) begin
                        nAck <= 1;
                    end
                end

                STOP:begin
                    scl_enable <= 0;
                end

            endcase
            
        end

        else begin
            unique case(state)

                READY:begin
                    if (enable) begin
                        busy    <= 1;
                        addr_rw <= {slv_addr, RnW};
                        data_tx <= data_wr;
                        state   <= START; 
                    end
                    else begin
                        busy  <= 0;
                        state <= READY;
                    end
                end

                START:begin
                    busy <= 1;
                    sda_int <= addr_rw[bit_cnt];
                    state   <= COMMAND;
                end

                COMMAND:begin
                    if (bit_cnt == 0) begin
                        sda_int <= 1;
                        bit_cnt <= 7;
                        state   <= SLV_ACK1;
                    end
                    else begin
                        bit_cnt <= bit_cnt -1;
                        sda_int <= addr_rw[bit_cnt];
                        state   <= COMMAND;
                    end
                end

                SLV_ACK1:begin
                    if (addr_rw[0] == 0) begin              // If WRITE COMMAND
                        sda_int <= data_tx[bit_cnt];        // Write first bit of data
                        state   <= WRITE;                   // Go to Write state
                    end
                    else begin
                        sda_int <= 1;                       // If it is not WRITE COMMAND, release SDA
                        state   <= READ;                    // and go to RD state
                    end
                end

                WRITE:begin
                    busy <= 1;
                    if (bit_cnt == 0) begin
                        sda_int <= 1;
                        bit_cnt <= 7;
                        state   <= SLV_ACK2;
                    end
                    else begin
                        bit_cnt <= bit_cnt -1;
                        sda_int <= data_tx[bit_cnt];
                        state   <= WRITE;
                    end
                end

                READ:begin
                    busy <= 1;
                    if (bit_cnt == 0) begin
                        if ((enable == 1) & (addr_rw == {slv_addr , RnW} )) begin
                            sda_int <= 0;
                        end
                        else begin
                            sda_int <= 1;
                        end
                        bit_cnt <= 7;
                        rd_data <= data_rx;
                        state   <= MSTR_ACK;
                    end
                    else begin
                        bit_cnt <= bit_cnt -1;
                        state   <= READ;
                    end
                end

                SLV_ACK2:begin
                    if (enable == 1) begin
                        busy <= 0;
                        addr_rw <= {slv_addr, RnW};
                        data_tx <= data_wr;

                        if (addr_rw == {slv_addr, RnW}) begin
                            sda_int <= data_wr[bit_cnt];
                            state <= WRITE;
                        end
                        else begin
                            state <= START;
                        end
                    end
                    else begin
                        state <= STOP;
                    end
                    
                end

                MSTR_ACK:begin
                    if (enable == 1) begin
                        busy <= 0;
                        addr_rw <= {slv_addr, RnW};
                        data_tx <= data_wr;

                        if (addr_rw == {slv_addr , RnW}) begin
                            sda_int <= 1;
                            state <= READ;
                        end
                        else begin
                            state <= START;
                        end
                    end
                    else begin
                        state <= STOP;
                    end

                end
                
                STOP:begin
                    busy  <= 0;
                    state <= READY;
                end

            endcase
            
        
        end       
        
    end: data_transaction_machine

    always_comb begin
        unique case(state)
            START:begin
                sda_enable_n <= scl_clock_prev;
            end
            STOP:begin
                sda_enable_n <= ~scl_clock_prev;
            end
            default: sda_enable_n <= sda_int;

        endcase
    end
    
    
    /*always_comb begin
    
        if (scl_enable==1 && (scl_clock == 0)) begin
            scl <= 1'b0;
        end    
        else begin
            scl <= 1'bZ; 
        end

    end*/

    assign scl = ((scl_enable==1) & (scl_clock == 0)) ? '0 : 'Z;
    assign sda = !sda_enable_n ? '0 : 'Z;


endmodule
