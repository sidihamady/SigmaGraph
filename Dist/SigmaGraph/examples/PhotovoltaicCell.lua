-- SigmaGraph script sample   http://www.hamady.org

clear()				-- Clear the output window

NN = 100			-- Number of points

dataT = Data:create()
dataT:new("Current")
nc,nr = dataT:dim()
if (nc < 3) then
	dataT:appendcol()
end
if (nr < NN) then
	dataT:appendrow(NN - nr)
end

dataT:format("B", "%.3f")
dataT:format("B", "%.4e")
dataT:format("C", "%.4e")

V = 0					-- Voltage (V)
T = 300				-- Temperature (K)
ISC = 1e-4			-- Short-circuit current (A)
RP = 1e6			-- Parallel resistance (Ohms)
IS1 = 1e-11		-- Ideality factor for diode 1
n1 = 1				-- Ideality factor for diode 2
IS2 = 1e-9			-- Saturation current for diode 2 (A)
n2 = 2				-- Ideality factor for diode 2
RS1 = 10			-- Series resistance (Ohms)
RS2 = 50			-- Series resistance (Ohms)
I0 = 1				-- Current max value (for the nonlinear solver)

Vinit = 0
Vfin = 0.5

DV = (Vfin - Vinit) / NN

for ii=1,NN do
	I1 = Physics.current(V, T, ISC, RP, IS1, n1, IS2, n2, RS1, I0)
	I1 = abs(I1)
	I2 = Physics.current(V, T, ISC, RP, IS1, n1, IS2, n2, RS2, I0)
	I2 = abs(I2)
	dataT:set("A", ii, V)
	dataT:set("B", ii, I1)
	dataT:set("C", ii, I2)
	V = V + DV
end

plotT = Plot:create()
plotT:add("A", "B", 1)
plotT:add("A", "C", 1)
plotT:axis(1, 1, 1, 0, 0, 1, "Voltage (V)")
plotT:axis(2, 2, 1, 0, 0, 1, "Current (A)")
plotT:color(1,255,0,0)
plotT:color(2,0,0,255)
plotT:update()

