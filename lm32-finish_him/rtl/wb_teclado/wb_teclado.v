`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    08:03:18 05/27/2015 
// Design Name: 
// Module Name:    LogicaTeclado 
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
module wb_teclado (		//Ojo que acá pongo el # porque me está botando error el sintetizador, cuando el system.v llama este modulo, pero me bota error, cuando lo pongo, y con razón, porque ese # es para definir parámetros...
	
    input clk_in_1,
	 input col_1,
	 input col_2,
	 input col_3,
	 input reset,
    output reg[6:0] segments,
	 output reg[3:0] value,
	 output reg[3:0] catodo,
	 output reg fil_1,
	 output reg fil_2,
	 output reg fil_3,
	 output reg fil_4,
	// Wishbone bus
	input      [31:0]   wb_adr_i,
	input      [31:0]   wb_dat_i,  	// este no lo voy a utilizar porque no voy a leer nada.
	output reg [31:0]   wb_dat_o,
	input      [ 3:0]   wb_sel_i,	// Este no se utiliza en ninguno de los .v
	input               wb_cyc_i,
	input               wb_stb_i,
	output              wb_ack_o,
	input               wb_we_i
    );
	 
	 Clock JoacoClk(			
		.clk_in(clk_in_1),
		.clk_out(clk_in_2)
		);

 	reg  ack;						//Logica wishbone
	assign wb_ack_o = wb_stb_i & wb_cyc_i & ack;		//Logica wishbone 
	wire wb_rd = wb_stb_i & wb_cyc_i & ~ack & ~wb_we_i;	//Logica wishbone // como metí el ack acá, en el if de escritura no lo meto
	wire wb_wr = wb_stb_i & wb_cyc_i & ~ack & wb_we_i;	//Logica wishbone // como metí el ack acá, en el if de escritura no lo meto
	 
	 
	 parameter state_1 = 3'b001;
	 parameter state_2 = 3'b010;
	 parameter state_3 = 3'b011;
	 parameter state_4 = 3'b100;
	 reg [5:0] counter; // Lo máximo es 93
	 
	 reg [2:0]state_next = 3'b000;
	
	//always @(posedge clk_in_2, posedge reset)
		//begin				
			//	state_next <= state_1;
				//value <= 4'd0;
		//end
	
		
	
	 always@(posedge clk_in_2)
		begin
			catodo <= 4'b0000;
			if (reset)// No sé porque rayos este reset me quedó invertido respecto a los reset del GPIO y uart.
				begin
						ack <= 0;		//Logica wishbone
						fil_1 <= 0;
						fil_2 <= 0;
						fil_3 <= 0;
						fil_4 <= 0;
						segments <= 7'b1111111;
						value <= 4'b0000;
						counter <= 6'b000000; /// Lo metí para el wishbone
				end
	
				else begin
					case(state_next)
						state_1:
								begin
									fil_1 <= 1;
									fil_2 <= 0;
									fil_3 <= 0;
									fil_4 <= 0;
									if (col_1 == 1)
										begin
											segments <= 7'b1001111;//1
											value <= 4'b0001;
											 counter <= 6'b000000;
										end
									else begin
										if (col_2 == 1)
											begin
												segments <= 7'b0010010;//2
												value <= 4'b0010;
												counter <= 6'b000000;			
											end
										else begin
											if (col_3 == 1)
												begin
													segments <= 7'b0000110;//3
													value <= 4'b0011;
													counter <= 6'b000000;		
												end
											else begin
												segments <= 7'b1111111;//default
												value <= 4'b0000;
												counter <= counter + 6'b000001;		
											end
										end
									end
							state_next = state_2;
						end
				
				
						state_2:
							begin
								fil_1 <= 0;
								fil_2 <= 1;
								fil_3 <= 0;
								fil_4 <= 0;
								if (col_1 == 1)
									begin
										segments <= 7'b1001100;//4
										value <= 4'b0100;
										counter <= 6'b000000;					
									end
								else begin
									if (col_2 == 1)
										begin
											segments <= 7'b0100100;//5
											value <= 4'b0101;
											counter <= 6'b000000;
										end
										else begin
											if (col_3 == 1)
												begin
													segments <= 7'b0100000;//6
													value <= 4'b0110;
													counter <= 6'b000000;
												end
												else begin
													segments <= 7'b1111111;//default
													value <= 4'b0000;
													counter <= counter + 6'b000001;	
												end
										end
								end
								state_next = state_3;
							end
					
						state_3:
							begin
								fil_1 <= 0;
								fil_2 <= 0;
								fil_3 <= 1;
								fil_4 <= 0;
								if (col_1 == 1)
									begin
										segments <= 7'b0001111;//7
										value <= 4'b0111;
										counter <= 6'b000000;	
									end
										else begin
											if (col_2 == 1)
												begin
													segments <= 7'b0000000;//8
													value <= 4'b1000;
													counter <= 6'b000000;
												end
													else begin
														if (col_3 == 1)
															begin
																segments <= 7'b0000100;//9
																value <= 4'b1001;
																counter <= 6'b000000;
															end
															else begin
																segments <= 7'b1111111;//default
																value <= 4'b0000;
																counter <= counter + 6'b000001;		
															end
													end
										end
										state_next = state_4;
							end
					
						state_4:
							begin
								fil_1 <= 0;
								fil_2 <= 0;
								fil_3 <= 0;
								fil_4 <= 1;
								if (col_1 == 1)
									begin
										segments <= 7'b0001000;//A
										value <= 4'b1111;
										counter <= 6'b000000;					
									end
									else begin
										if (col_2 == 1)
											begin
												segments <= 7'b0000001;//B
												value <= 4'b0000;
												counter <= 6'b000000;					
											end
												else begin
													if (col_3 == 1)
														begin
															segments <= 7'b0110001;//C
															value <= 4'b1111;
															counter <= 6'b000000;		
														end
														else begin
															segments <= 7'b1111111;//default
															value <= 4'b0000;
															counter <= counter + 6'b000001;				
														end
												end
									end
									state_next = state_1; 
							end
						default: state_next = state_1;				
					endcase

				
					ack <= wb_stb_i & wb_cyc_i;					//Logica wishbone
					if ((wb_rd && value[3:0] != 4'b0000) || counter > 10)							//Logica wishbone
						begin           			// read cycle	//Logica wishbone
							case (wb_adr_i[3:0] )		//Que espacio del arreglo dejo acá?  ---> creo que no interesa, porque las direcciones van muy rápido //Logica wishbone
								4'b0000:begin 
									wb_dat_o [31:4]<= 28'b0;
									wb_dat_o [3:0]<= value[3:0];		//Logica wishbone, acá en vez de value, creo que puedo poner cualquier valor, pongamos 1 haber si lo lee en el UART...
									end					//Logica wishbone
							endcase						//Logica wishbone
						end
					if(counter > 63)
						begin
							counter <= 6'b000000;
						end
												//Logica wishbone


				end
		end
endmodule
		
