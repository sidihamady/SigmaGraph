-- SigmaGraph script sample   http://www.hamady.org

clear()										-- Clear the output window

NN = 100

dataT = Data:create()
dataT:new("Admittance")
nc,nr = dataT:dim()
if (nc < 3) then
	dataT:appendcol()
end
if (nr < NN) then
	dataT:appendrow(NN - nr)
end

dataT:format("A", "%.3e")
dataT:format("B", "%.4e")
dataT:format("C", "%.4e")

FreqInit = 1e3
FreqFin = 1e6
DFreq = ln(FreqFin / FreqInit) / NN

r = 100					-- Equivalent series inductance
L = 1e-6				-- Series resistance
GLF = 1e-7				-- LF conductance
CHF = 1e-9				-- HF capacitance
C1 = 1e-9				-- Capacitance - Level 1
Tau1 = 1e-4			-- Time constant - Level 1
C2 = 1e-9				-- Capacitance - Level 2
Tau2 = 1e-5			-- Time constant - Level 1

Freq = FreqInit
for ii=1,NN do
	Cm = Physics.capacitance(Freq, r, L, GLF, CHF, C1, Tau1, C2, Tau2)
	Gm = Physics.conductance(Freq, r, L, GLF, CHF, C1, Tau1, C2, Tau2)
	dataT:set("A", ii, Freq)
	dataT:set("B", ii, Cm)
	dataT:set("C", ii, Gm)
	Freq = FreqInit * exp(ii * DFreq)
end

plotT = Plot:create()
plotT:add("A", "B", 1, 1)
plotT:add("A", "C", 1, 2)
plotT:axis(1, 2, 1, 0, 0, 1, "Frequency (Hz)")
plotT:axis(2, 1, 1, 0, 0, 1, "Capacitance (F)")
plotT:axis(4, 1, 1, 0, 0, 1, "Conductance/w (F)")
plotT:label(4, 1)
plotT:color(1,255,0,0)
plotT:color(2,0,0,255)
plotT:update()
