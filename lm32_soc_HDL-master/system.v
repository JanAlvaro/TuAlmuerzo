//---------------------------------------------------------------------------
// LatticeMico32 System On A Chip
//
// Top Level Design for the Nexys4 ferney
//---------------------------------------------------------------------------

module system
#(
//	parameter   bootram_file     = "../firmware/cain_loader/image.ram",
//	parameter   bootram_file     = "../firmware/arch_examples/image.ram",
//	parameter   bootram_file     = "../firmware/boot0-serial/image.ram",
	parameter   bootram_file     = "../firmware/hw-test/image.ram",
	parameter   clk_freq         = 100000000,
	parameter   uart_baud_rate   = 115200
) (
	input             clk,
	// Debug 
	//output            led,
	input             rst,

	// UART0
	input             uart_rxd, 
	output            uart_txd,
	// UART1
	input             uart_rxd1, 
	output            uart_txd1,


	// SPI
	input             miso, 
	output            mosi,
	output            sck,
	output		ss,
	// 12c
	inout             i2c_sda, 
	inout             i2c_scl,
	// GPIO
	output            [7:0] gpio0_io,
	//TECLADO
	 input col_1,
	 input col_2,
	 input col_3,
	output [6:0] segments0,   //Siguiendo la wea de arriba a este registro de salida le quito el reg, importante: segments0, pues los wire no se modificaban al final de este archivo.
	 output [3:0] value0,//Siguiendo la wea de arriba a este registro de salida le quito el reg, importante: value0.
	 output [3:0] catodo0,//Siguiendo la wea de arriba a este registro de salida le quito el reg, importante: catodo0.
	 output fil_1,//Siguiendo la wea de arriba a este registro de salida le quito el reg
	 output fil_2,//Siguiendo la wea de arriba a este registro de salida le quito el reg
	 output fil_3,//Siguiendo la wea de arriba a este registro de salida le quito el reg
	 output fil_4//Siguiendo la wea de arriba a este registro de salida le quito el reg

);


wire sys_clk = clk;
wire sys_clk_n = ~clk;


	
//------------------------------------------------------------------
// Whishbone Wires
//------------------------------------------------------------------
wire         gnd   =  1'b0;
wire   [3:0] gnd4  =  4'h0;
wire  [31:0] gnd32 = 32'h00000000;

 
wire [31:0]  lm32i_adr,
             lm32d_adr,
             uart0_adr,
             uart1_adr,
             spi0_adr,
             i2c0_adr,
             timer0_adr,
             gpio0_adr,
		teclado0_adr,//Cosa que cambié para leer teclado
             ddr0_adr,
             bram0_adr,
             sram0_adr;


wire [31:0]  lm32i_dat_r,
             lm32i_dat_w,
             lm32d_dat_r,
             lm32d_dat_w,
             uart0_dat_r,
             uart1_dat_r,
             uart0_dat_w,
             uart1_dat_w,
             spi0_dat_r,
             spi0_dat_w,
             i2c0_dat_r,
             i2c0_dat_w,
             timer0_dat_r,
             timer0_dat_w,
             gpio0_dat_r,
             gpio0_dat_w,
		teclado0_dat_r,//Cosa que cambié para leer teclado
		teclado0_dat_w,//Cosa que cambié para leer teclado
             bram0_dat_r,
             bram0_dat_w,
             sram0_dat_w,
             sram0_dat_r,
             ddr0_dat_w,
             ddr0_dat_r;

wire [3:0]   lm32i_sel,
             lm32d_sel,
             uart0_sel,
             uart1_sel,
             spi0_sel,
             i2c0_sel,
             timer0_sel,
             gpio0_sel,
		teclado0_sel,//Cosa que cambié para leer teclado
             bram0_sel,
             sram0_sel,
             ddr0_sel;

wire         lm32i_we,
             lm32d_we,
             uart0_we,
             uart1_we,
             spi0_we,
             i2c0_we,
             timer0_we,
             gpio0_we,
		teclado0_we,//Cosa que cambié para leer teclado
             bram0_we,
             sram0_we,
             ddr0_we;


wire         lm32i_cyc,
             lm32d_cyc,
             uart0_cyc,
             uart1_cyc,
//             spi0_cyc,
             i2c0_cyc,
             timer0_cyc,
             gpio0_cyc,
		teclado0_cyc,//Cosa que cambié para leer teclado
             bram0_cyc,
             sram0_cyc,
             ddr0_cyc;


wire         lm32i_stb,
             lm32d_stb,
             uart0_stb,
             uart1_stb,
             spi0_stb,
             i2c0_stb,
             timer0_stb,
             gpio0_stb,
		teclado0_stb,//Cosa que cambié para leer teclado
             bram0_stb,
             sram0_stb,
             ddr0_stb;

wire         lm32i_ack,
             lm32d_ack,
             uart0_ack,
             uart1_ack,
             spi0_ack,
             i2c0_ack,
             timer0_ack,
             gpio0_ack,
		teclado0_ack,//Cosa que cambié para leer teclado
             bram0_ack,
             sram0_ack,
             ddr0_ack;


wire         lm32i_rty,
             lm32d_rty;

wire         lm32i_err,
             lm32d_err;

wire         lm32i_lock,
             lm32d_lock;

wire [2:0]   lm32i_cti,
             lm32d_cti;

wire [1:0]   lm32i_bte,
             lm32d_bte;

//---------------------------------------------------------------------------
// Interrupts
//---------------------------------------------------------------------------
wire [31:0]  intr_n;
wire         uart0_intr = 0;// Esto lo omito para uart1
wire   [1:0] timer0_intr;
wire         gpio0_intr;

assign intr_n = { 28'hFFFFFFF, ~timer0_intr[1], ~gpio0_intr, ~timer0_intr[0], ~uart0_intr };// Esto lo omito para uart1

//---------------------------------------------------------------------------
// Wishbone Interconnect
//---------------------------------------------------------------------------
conbus #(
	.s_addr_w(3),
	.s0_addr(3'b000),	// bram     0x00000000 
	.s1_addr(3'b010),	// uart0    0x20000000  
	.s2_addr(3'b011),	// timer    0x30000000 	
	.s3_addr(3'b111),   	// gpio// En el soc-hw.c modificado para leer uart1 0x70000000 	
	.s4_addr(3'b101),	// spi      0x50000000 
	.s5_addr(3'b110)		// i2c// En el soc-hw.c modificado para leer teclado  0x60000000
) conbus0(
	.sys_clk( clk ),
	.sys_rst( ~rst ),
	// Master0
	.m0_dat_i(  lm32i_dat_w  ),
	.m0_dat_o(  lm32i_dat_r  ),
	.m0_adr_i(  lm32i_adr    ),
	.m0_we_i (  lm32i_we     ),
	.m0_sel_i(  lm32i_sel    ),
	.m0_cyc_i(  lm32i_cyc    ),
	.m0_stb_i(  lm32i_stb    ),
	.m0_ack_o(  lm32i_ack    ),
	// Master1
	.m1_dat_i(  lm32d_dat_w  ),
	.m1_dat_o(  lm32d_dat_r  ),
	.m1_adr_i(  lm32d_adr    ),
	.m1_we_i (  lm32d_we     ),
	.m1_sel_i(  lm32d_sel    ),
	.m1_cyc_i(  lm32d_cyc    ),
	.m1_stb_i(  lm32d_stb    ),
	.m1_ack_o(  lm32d_ack    ),


	// Slave0  bram
	.s0_dat_i(  bram0_dat_r ),
	.s0_dat_o(  bram0_dat_w ),
	.s0_adr_o(  bram0_adr   ),
	.s0_sel_o(  bram0_sel   ),
	.s0_we_o(   bram0_we    ),
	.s0_cyc_o(  bram0_cyc   ),
	.s0_stb_o(  bram0_stb   ),
	.s0_ack_i(  bram0_ack   ),

	// Slave1
	.s1_dat_i(  uart0_dat_r ),
	.s1_dat_o(  uart0_dat_w ),
	.s1_adr_o(  uart0_adr   ),
	.s1_sel_o(  uart0_sel   ),
	.s1_we_o(   uart0_we    ),
	.s1_cyc_o(  uart0_cyc   ),
	.s1_stb_o(  uart0_stb   ),
	.s1_ack_i(  uart0_ack   ),

	// Slave2
	.s2_dat_i(  timer0_dat_r ),
	.s2_dat_o(  timer0_dat_w ),
	.s2_adr_o(  timer0_adr   ),
	.s2_sel_o(  timer0_sel   ),
	.s2_we_o(   timer0_we    ),
	.s2_cyc_o(  timer0_cyc   ),
	.s2_stb_o(  timer0_stb   ),
	.s2_ack_i(  timer0_ack   ),

//LO siguiente lo quito para conectar el UART1 a continuación
/*
	// Slave3

	.s3_dat_i(  gpio0_dat_r ),
	.s3_dat_o(  gpio0_dat_w ),
	.s3_adr_o(  gpio0_adr   ),
	.s3_sel_o(  gpio0_sel   ),
	.s3_we_o(   gpio0_we    ),
	.s3_cyc_o(  gpio0_cyc   ),
	.s3_stb_o(  gpio0_stb   ),
	.s3_ack_i(  gpio0_ack   ),
*/
	.s3_dat_i(  uart1_dat_r ),
	.s3_dat_o(  uart1_dat_w ),
	.s3_adr_o(  uart1_adr   ),
	.s3_sel_o(  uart1_sel   ),
	.s3_we_o(   uart1_we    ),
	.s3_cyc_o(  uart1_cyc   ),
	.s3_stb_o(  uart1_stb   ),
	.s3_ack_i(  uart1_ack   ),

	// Slave4
	.s4_dat_i(  spi0_dat_r ),
	.s4_dat_o(  spi0_dat_w ),
	.s4_adr_o(  spi0_adr   ),
	.s4_sel_o(  spi0_sel   ),
	.s4_we_o(   spi0_we    ),
	.s4_cyc_o(  spi0_cyc   ),
	.s4_stb_o(  spi0_stb   ),
	.s4_ack_i(  spi0_ack   ),
	// Slave5
//LO siguiente lo quito para conectar el teclado a continuación
/*
	.s5_dat_i(  i2c0_dat_r ),
	.s5_dat_o(  i2c0_dat_w ),
	.s5_adr_o(  i2c0_adr   ),
	.s5_sel_o(  i2c0_sel   ),
	.s5_we_o(   i2c0_we    ),
	.s5_cyc_o(  i2c0_cyc   ),
	.s5_stb_o(  i2c0_stb   ),
	.s5_ack_i(  i2c0_ack   )
*/

	.s5_dat_i(  teclado0_dat_r ),
	.s5_dat_o(  teclado0_dat_w ),
	.s5_adr_o(  teclado0_adr   ),
	.s5_sel_o(  teclado0_sel   ),
	.s5_we_o(   teclado0_we    ),
	.s5_cyc_o(  teclado0_cyc   ),
	.s5_stb_o(  teclado0_stb   ),
	.s5_ack_i(  teclado0_ack   )

);


//---------------------------------------------------------------------------
// LM32 CPU 
//---------------------------------------------------------------------------
lm32_cpu lm0 (
	.clk_i(  clk  ),
	.rst_i(  ~rst  ),
	.interrupt_n(  intr_n  ),
	//
	.I_ADR_O(  lm32i_adr    ),
	.I_DAT_I(  lm32i_dat_r  ),
	.I_DAT_O(  lm32i_dat_w  ),
	.I_SEL_O(  lm32i_sel    ),
	.I_CYC_O(  lm32i_cyc    ),
	.I_STB_O(  lm32i_stb    ),
	.I_ACK_I(  lm32i_ack    ),
	.I_WE_O (  lm32i_we     ),
	.I_CTI_O(  lm32i_cti    ),
	.I_LOCK_O( lm32i_lock   ),
	.I_BTE_O(  lm32i_bte    ),
	.I_ERR_I(  lm32i_err    ),
	.I_RTY_I(  lm32i_rty    ),
	//
	.D_ADR_O(  lm32d_adr    ),
	.D_DAT_I(  lm32d_dat_r  ),
	.D_DAT_O(  lm32d_dat_w  ),
	.D_SEL_O(  lm32d_sel    ),
	.D_CYC_O(  lm32d_cyc    ),
	.D_STB_O(  lm32d_stb    ),
	.D_ACK_I(  lm32d_ack    ),
	.D_WE_O (  lm32d_we     ),
	.D_CTI_O(  lm32d_cti    ),
	.D_LOCK_O( lm32d_lock   ),
	.D_BTE_O(  lm32d_bte    ),
	.D_ERR_I(  lm32d_err    ),
	.D_RTY_I(  lm32d_rty    )
);
	
//---------------------------------------------------------------------------
// Block RAM
//---------------------------------------------------------------------------
wb_bram #(
	.adr_width( 12 ),
	.mem_file_name( bootram_file )
) bram0 (
	.clk_i(  clk  ),
	.rst_i(  ~rst  ),
	//
	.wb_adr_i(  bram0_adr    ),
	.wb_dat_o(  bram0_dat_r  ),
	.wb_dat_i(  bram0_dat_w  ),
	.wb_sel_i(  bram0_sel    ),
	.wb_stb_i(  bram0_stb    ),
	.wb_cyc_i(  bram0_cyc    ),
	.wb_ack_o(  bram0_ack    ),
	.wb_we_i(   bram0_we     )
);



//---------------------------------------------------------------------------
// uart0
//---------------------------------------------------------------------------
wire uart0_rxd;
wire uart0_txd;

wb_uart #(
	.clk_freq( clk_freq        ),
	.baud(     uart_baud_rate  )
) uart0 (
	.clk( clk ),
	.reset( ~rst ),
	//
	.wb_adr_i( uart0_adr ),
	.wb_dat_i( uart0_dat_w ),
	.wb_dat_o( uart0_dat_r ),
	.wb_stb_i( uart0_stb ),
	.wb_cyc_i( uart0_cyc ),
	.wb_we_i(  uart0_we ),
	.wb_sel_i( uart0_sel ),
	.wb_ack_o( uart0_ack ), 
//	.intr(       uart0_intr ),
	.uart_rxd( uart0_rxd ),
	.uart_txd( uart0_txd )
);


//---------------------------------------------------------------------------
// uart1
//---------------------------------------------------------------------------
wire uart1_rxd;
wire uart1_txd;

wb_uart #(
	.clk_freq( clk_freq        ),
	.baud(     uart_baud_rate  )
) uart1 (
	.clk( clk ),
	.reset( ~rst ),
	//
	.wb_adr_i( uart1_adr ),
	.wb_dat_i( uart1_dat_w ),
	.wb_dat_o( uart1_dat_r ),
	.wb_stb_i( uart1_stb ),
	.wb_cyc_i( uart1_cyc ),
	.wb_we_i(  uart1_we ),
	.wb_sel_i( uart1_sel ),
	.wb_ack_o( uart1_ack ), 
//	.intr(       uart0_intr ),
	.uart_rxd( uart1_rxd ),
	.uart_txd( uart1_txd )
);

//---------------------------------------------------------------------------
// spi0
//---------------------------------------------------------------------------
wb_spi  spi0 (			
	.clk( clk ),
	.reset( ~rst ),
	//
	.wb_adr_i( spi0_adr ),
	.wb_dat_i( spi0_dat_w ),
	.wb_dat_o( spi0_dat_r ),
	.wb_stb_i( spi0_stb ),
	.wb_cyc_i( spi0_cyc ),
	.wb_we_i(  spi0_we ),
	.wb_sel_i( spi0_sel ),
	.wb_ack_o( spi0_ack ),
	.mosi(mosi),
	.miso(miso),
	.sck(sck),
	.ss(ss)
	
);


//---------------------------------------------------------------------------
// i2c0
//---------------------------------------------------------------------------
 wire i2c0_sda;
 wire i2c0_scl;

// TODO : interruption and asynchronous reset
 i2c_master_wb_top  i2c0 (
 	.wb_clk_i( clk ),
	.wb_rst_i( ~rst ),
	//
	.wb_adr_i( i2c0_adr ),
	.wb_dat_i( i2c0_dat_w ),
	.wb_dat_o( i2c0_dat_r ),
	.wb_stb_i( i2c0_stb ),
	.wb_cyc_i( i2c0_cyc ),
	.wb_we_i(  i2c0_we ),
	.wb_ack_o( i2c0_ack ), 
	.scl(i2c0_scl),
	.sda( i2c0_sda )
);

//---------------------------------------------------------------------------
// timer0
//---------------------------------------------------------------------------
wb_timer #(
	.clk_freq(   clk_freq  )
) timer0 (
	.clk(      clk          ),
	.reset(    ~rst          ),
	//
	.wb_adr_i( timer0_adr   ),
	.wb_dat_i( timer0_dat_w ),
	.wb_dat_o( timer0_dat_r ),
	.wb_stb_i( timer0_stb   ),
	.wb_cyc_i( timer0_cyc   ),
	.wb_we_i(  timer0_we    ),
	.wb_sel_i( timer0_sel   ),
	.wb_ack_o( timer0_ack   ), 
	.intr(     timer0_intr  )
);

//---------------------------------------------------------------------------
// General Purpose IO			
//---------------------------------------------------------------------------

wire [7:0] gpio0_io;
wire        gpio0_irq;

wb_gpio gpio0 (
	.clk(      clk          ),
	.rst(    ~rst          ),
	//
	.wb_adr_i( gpio0_adr    ),
	.wb_dat_i( gpio0_dat_w  ),
	.wb_dat_o( gpio0_dat_r  ),
	.wb_stb_i( gpio0_stb    ),
	.wb_cyc_i( gpio0_cyc    ),
	.wb_we_i(  gpio0_we     ),
	.wb_ack_o( gpio0_ack    ), 
	.gpio_io(gpio0_io)
);

//---------------------------------------------------------------------------
// TECLADO MATRICIAL		
//---------------------------------------------------------------------------
wire col0_1;
wire col0_2;
wire col0_3;
wire fil0_1;
wire fil0_2;
wire fil0_3;
wire fil0_4;
wire [6:0] segments0;
wire [3:0] value0;
wire [3:0] catodo0;

wb_teclado  teclado0 (
	.clk_in_1( clk ),
	.reset( ~rst ),         // Por este motivo era que me salía la señal del reset inversa :D, porque acá se invierte :D
	.col_1(col0_1),		//Input
	.col_2(col0_2),		//Input
	.col_3(col0_3),		//Input
	.segments(segments0),	//Output REG
	.value(value0),		//Output REG
	.catodo(catodo0),	//Output REG
	.fil_1(fil0_1),		//Output
	.fil_2(fil0_2),		//Output
	.fil_3(fil0_3),		//Output
	.fil_4(fil0_4),		//Output
	//
	.wb_adr_i( teclado0_adr    ),
	.wb_dat_i( teclado0_dat_w  ),
	.wb_dat_o( teclado0_dat_r  ),
	.wb_stb_i( teclado0_stb    ),
	.wb_cyc_i( teclado0_cyc    ),
	.wb_we_i(  teclado0_we     ),
	.wb_sel_i( teclado0_sel   ),
	.wb_ack_o( teclado0_ack    )
);

//----------------------------------------------------------------------------
// Mux UART wires according to sw[0]
//----------------------------------------------------------------------------

// Estas son algunas de las variables entradas o salidas que vienen de los .v como el spi, uart, i2c, teclado, que para este .v también se convierten en entradas y salidas.

//Salidas del teclado:
assign fil_1 = fil0_1;						// MOdificado para teclado
assign fil_2 = fil0_2;						// MOdificado para teclado
assign fil_3 = fil0_3;						// MOdificado para teclado
assign fil_4 = fil0_4;						// MOdificado para teclado
//assign segments = segments0;//Los registros con más de un bit (osea wire [#:#]) acá no les hacen asignación	// MOdificado para teclado
//assign value = value0;//Los registros con más de un bit (osea wire [#:#]) acá no les hacen asignación	// MOdificado para teclado
//assign catodo = catodo0;//Los registros con más de un bit (osea wire [#:#]) acá no les hacen asignación	// MOdificado para teclado

//Entradas del teclado
assign col0_1 = col_1;	// MOdificado para teclado
assign col0_2 = col_2;	// MOdificado para teclado
assign col0_3 = col_3;	// MOdificado para teclado



assign uart_txd  = uart0_txd;  //salida
assign uart0_rxd = uart_rxd;   //entrada
assign led       = ~uart_txd;

assign uart_txd1  = uart1_txd;  //salida			Esto lo puse para el UART1
assign uart1_rxd = uart_rxd1;   //entrada

//assign spi_mosi  = spi0_mosi;		//salida	
//assign spi0_miso = spi_miso;	//Entrada		
//assign spi_clk = spi0_clk;		//salida	
//assign run_cs = run0_cs;

assign i2c_sda = i2c0_sda;	//salida
assign i2c_scl = i2c0_scl;	//salida
endmodule 
