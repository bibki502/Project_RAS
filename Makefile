CXX = g++
CXXFLAGS = -Wall -O2
TARGET = prog
VERILOG_GEN = verilog.v
TB = tb_karatsuba.v
SIM = sim.vvp

all: run

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)


run: $(TARGET)
	./$(TARGET)
	iverilog -o $(SIM) $(VERILOG_GEN) $(TB)
	vvp $(SIM)


clean:
	rm -f $(TARGET) $(SIM) $(VERILOG_GEN) wave.vcd

.PHONY: all run clean