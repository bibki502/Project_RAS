# Значение N можно менять при вызове make
N ?= 16

CXX = g++
CXXFLAGS = -Wall -O2
TARGET = prog
VERILOG_GEN = verilog.v
TB = tb_karatsuba.v
SIM = sim.vvp

all: build run

build: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)


run:
	@echo "$(N)" | ./$(TARGET)
	iverilog -P tb_karatsuba.N=$(N) -o $(SIM) $(VERILOG_GEN) $(TB)
	vvp $(SIM)

clean:
	rm -f $(TARGET) $(SIM) $(VERILOG_GEN) wave.vcd

.PHONY: all build run clean