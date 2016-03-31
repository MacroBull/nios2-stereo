 
SRC = ins_census.v ins_census_tb.v
#fixPoint.v complex.v fractal_i.v
# top.sv hexTest.v svTest.sv

VC = iverilog
VP = vvp
WV = gtkwave

FLAG = 
# -g system-verilog


build: wave

compile: 
	$(VC) $(FLAG) -o out.vvp $(SRC)

simulation: compile
	$(VP) out.vvp

wave: simulation
	ps -ea | grep $(WV) || sh -c "$(WV) *.vcd &"

clean:
	-rm out.vvp *.vcd
