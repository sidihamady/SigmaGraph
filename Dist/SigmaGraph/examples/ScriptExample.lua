-- SigmaGraph script sample

NN = 100					-- Number of points

dataT = Data:create()				-- Create a Data object
dataT:new("SigmaGraph")				-- Create a new datasheet
nc,nr = dataT:dim()				-- Get the number of column and rows
if (nc < 3) then				-- Append one column, if necessary
	dataT:appendcol()
end
if (nr < NN) then				-- Append rows, if necessary
	dataT:appendrow(NN - nr)
end
dataT:format("A", "%.3f")			-- Set numeric format to scientific...
dataT:format("B", "%.3f")			-- ... for columns A, B and C
dataT:format("C", "%.3f")

Xinit = 0					-- Initial value
Xstep = 0.05					-- Step value
X = Xinit
for ii=1,NN do
	Y1 = 1 - exp(-X)
	Y2 = 1 - exp(-2 * X)
	dataT:set("A", ii, X)
	dataT:set("B", ii, Y1)
	dataT:set("C", ii, Y2)
	X = X + Xstep
end

plotT = Plot:create()				-- Create a Plot object
plotT:add("A", "B", 1, 1)			-- Add Y1(X) curve
plotT:add("A", "C", 1, 1)			-- Add Y2(X) curve

-- Set X-axis properties
plotT:axis(1, 1, 1, 0, 0, 1, "Time (a.u.)")

-- Set Y-axis properties
plotT:axis(2, 1, 1, 0, 0, 1, "Charge (a.u.)")

plotT:color(1,255,0,0)				-- Curve color (red)
plotT:color(2,0,0,255)				-- Curve color (blue)
plotT:update()					-- Update the graph
