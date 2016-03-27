	component soc is
		port (
			clk_clk       : in  std_logic                     := 'X'; -- clk
			reset_reset_n : in  std_logic                     := 'X'; -- reset_n
			seg6_export   : out std_logic_vector(23 downto 0);        -- export
			uart_rxd      : in  std_logic                     := 'X'; -- rxd
			uart_txd      : out std_logic                             -- txd
		);
	end component soc;

	u0 : component soc
		port map (
			clk_clk       => CONNECTED_TO_clk_clk,       --   clk.clk
			reset_reset_n => CONNECTED_TO_reset_reset_n, -- reset.reset_n
			seg6_export   => CONNECTED_TO_seg6_export,   --  seg6.export
			uart_rxd      => CONNECTED_TO_uart_rxd,      --  uart.rxd
			uart_txd      => CONNECTED_TO_uart_txd       --      .txd
		);

