`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    00:44:39 05/27/2015 
// Design Name: 
// Module Name:    Clock 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module Clock(
		input clk_in,
		output reg clk_out
		);
		
		
		reg [7:0] counter = 0;
		initial clk_out <= 0;

		always@(posedge clk_in)
			begin
				if(counter == 200)
					begin
						clk_out <= ~clk_out;
						counter = 0;
					end
				else
					begin
						counter = counter + 1;
					end

			end
			
endmodule
