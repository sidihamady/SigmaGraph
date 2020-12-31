// ----------------------------------------------------------------------------------------------
// SigmaGraph
// Data Plotting and Analysis Software
// Copyright(C) 1997-2020  Pr. Sidi HAMADY
// http://www.hamady.org
// sidi@hamady.org
// Released under the MIT License:
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files(the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions :
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
// AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----------------------------------------------------------------------------------------------

// :TODO: complete doc and translate to english

#include "stdafx.h"

#include "Sigma.h"
#include "ScriptWindow.h"

const scriptsample_t CScriptWindow::SCRIPT_SAMPLES[] = {

    {
        _T("Plot function"),

        18,
        16,

        _T("-- SigmaGraph script sample\r\n\r\n")

        _T("cls()                         -- clear the Output Window\r\n")
        _T("tD = Data:create()            -- create an instance of the Data class\r\n")
        _T("tD:new(\"SigmaGraph\")        -- create a new datasheet named SigmaGraph\r\n\r\n")

        _T("dx = 0.1\r\n")
        _T("x = {}\r\n")
        _T("y = {}\r\n")
        _T("for ii = 1, 11 do             -- filling columns A and B\r\n")
        _T("   x[ii] = dx * (ii - 1)\r\n")
        _T("   y[ii] = 1 - exp(-5*x[ii])\r\n")
        _T("end\r\n")
        _T("tD:set(\"A\", x)\r\n")
        _T("tD:set(\"B\", y)\r\n\r\n")

        _T("tP = Plot:create()            -- create an instance of the Plot class\r\n")
        _T("tP:add(\"A\", \"B\", 1)       -- add B versus A curve to the graph, Line style\r\n")
        _T("tP:update()                   -- Update the graph\r\n")
    },

    {
        _T("Plot options"),
        
        45,
        16,

        _T("-- SigmaGraph script sample\r\n\r\n")

        _T("NN = 100    --Number of points\r\n")

        _T("tD = Data:create()              -- Create a Data object\r\n")
        _T("tD:new(\"SigmaGraph\")          -- Create a new datasheet\r\n")
        _T("nc, nr = tD:dim()               -- Get the number of column and rows\r\n\r\n")

        _T("if (nc < 3) then                -- Append one column, if necessary\r\n")
        _T("   tD:appendcol()\r\n")
        _T("end\r\n\r\n")

        _T("if (nr < NN) then               -- Append rows, if necessary\r\n")
        _T("   tD:appendrow(NN - nr)\r\n")
        _T("end\r\n\r\n")

        _T("tD:format(\"A\", \"%.3f\")          -- Set numeric format to scientific...\r\n")
        _T("tD:format(\"B\", \"%.3f\")          -- ... for columns A, B and C\r\n")
        _T("tD:format(\"C\", \"%.3f\")\r\n")
        _T("Xstep = 0.05                        -- Step value\r\n\r\n")

        _T("x = {}\r\n")
        _T("y1 = {}\r\n")
        _T("y2 = {}\r\n")
        _T("for ii = 1, NN do\r\n")
        _T("   x[ii] = Xstep * (ii - 1)\r\n")
        _T("   y1[ii] = 1 - exp(-x[ii])\r\n")
        _T("   y2[ii] = 1 - exp(-2 * x[ii])\r\n")
        _T("end\r\n")
        _T("tD:set(\"A\", x)\r\n")
        _T("tD:set(\"B\", y1)\r\n")
        _T("tD:set(\"C\", y2)\r\n\r\n")

        _T("tP = Plot:create()                   -- Create a Plot object\r\n")
        _T("tP:add(\"A\", \"B\", 1, 1)           -- Add Y1(X) curve\r\n")
        _T("tP:add(\"A\", \"C\", 1, 1)           -- Add Y2(X) curve\r\n\r\n")

        _T("-- Set X - axis properties\r\n")
        _T("tP:axis(1, 1, 1, 0, 0, 1, \"Time(a.u.)\")\r\n\r\n")

        _T("-- Set Y - axis properties\r\n")
        _T("tP:axis(2, 1, 1, 0, 0, 1, \"Charge(a.u.)\")\r\n")
        _T("tP:color(1, 255, 0, 0)           -- Curve color(red)\r\n")
        _T("tP:color(2, 0, 0, 255)           -- Curve color(blue)\r\n")
        _T("tP:update()                      -- Update the graph\r\n")
    },

    {
        _T("Curve Fitting"),

        33,
        4,

        _T("-- SigmaGraph script sample\r\n\r\n")

        _T("tD = Data:create()             -- Create a Data object\r\n")
        _T("tD:new(\"Fitting\")            -- Create a new datasheet\r\n")
        _T("nc, nr = tD:dim()              -- Get the number of column and rows\r\n\r\n")

        _T("if (nc < 3) then               -- Append one column, if necessary\r\n")
        _T("   tD:appendcol()\r\n")
        _T("end\r\n\r\n")

        _T("x = {}\r\n")
        _T("y = {}\r\n")
        _T("z = {}\r\n")
        _T("a = 1\r\n")
        _T("b = 2\r\n")
        _T("for ii = 1, 10 do\r\n")
        _T("   x[ii] = (ii - 1) * 0.1\r\n")
        _T("   y[ii] = a + b * x[ii] + 0.3 * math.random()\r\n")
        _T("end\r\n\r\n")

        _T("pars, chi, iers, msg = data.fit(\"Linear\", x, y, {0.1,0.1})\r\n\r\n")

        _T("for ii = 1, 10 do\r\n")
        _T("    z[ii] = pars[1] + pars[2] * x[ii]\r\n")
        _T("end\r\n\r\n")

        _T("tD:set(\"A\", x)\r\n")
        _T("tD:set(\"B\", y)\r\n")
        _T("tD:set(\"C\", z)\r\n\r\n")
    
        _T("tP = Plot:create()                 -- Create a Plot object\r\n")
        _T("tP:add(\"A\", \"B\", 2, 1)         -- Add data curve\r\n")
        _T("tP:add(\"A\", \"C\", 1, 1)         -- Add fit curve\r\n")
    },

    {
        _T("Photovoltaic cell"),

        60,
        43,

        _T("-- SigmaGraph script sample\r\n\r\n")

        _T("tic()       -- Start timer.\r\n")
        _T("cls()       -- Clear the output window\r\n")
        _T("NN = 100    -- Number of points\r\n\r\n")

        _T("tD = Data:create()\r\n")
        _T("tD:new(\"Current\")\r\n")
        _T("nc, nr = tD:dim()\r\n")
        _T("if (nc < 3) then\r\n")
        _T("  tD:appendcol()\r\n")
        _T("end\r\n")
        _T("if (nr < NN) then\r\n")
        _T("   tD:appendrow(NN - nr)\r\n")
        _T("end\r\n\r\n")

        _T("tD:format(\"A\", \"%.3f\")\r\n")
        _T("tD:format(\"B\", \"%.4e\")\r\n")
        _T("tD:format(\"C\", \"%.4e\")\r\n\r\n")

        _T("V = 0         -- Voltage(V)\r\n")
        _T("T = 300       -- Temperature(K)\r\n")
        _T("ISC = 1e-4    -- Short - circuit current(A)\r\n")
        _T("RP1 = 1e6     -- Parallel resistance(Ohms)\r\n")
        _T("RP2 = 1e4     -- Parallel resistance(Ohms)\r\n")
        _T("IS1 = 1e-11   -- Ideality factor for diode 1\r\n")
        _T("n1 = 1        -- Ideality factor for diode 2\r\n")
        _T("IS2 = 1e-9    -- Saturation current for diode 2 (A)\r\n")
        _T("n2 = 2        -- Ideality factor for diode 2\r\n")
        _T("RS1 = 10      -- Series resistance(Ohms)\r\n")
        _T("RS2 = 10      -- Series resistance(Ohms)\r\n")
        _T("I0 = 1        -- Current max value(for the nonlinear solver)\r\n")
        _T("Vinit = 0\r\n")
        _T("Vfin = 0.5\r\n")
        _T("DV = (Vfin - Vinit) / NN\r\n\r\n")

        _T("V = {}\r\n")
        _T("I1 = {}\r\n")
        _T("I2 = {}\r\n")
        _T("for ii = 1, NN do\r\n")
        _T("   V[ii] = DV * (ii - 1)\r\n")
        _T("   I1[ii] = Physics.current(V[ii], T, ISC, RP1, IS1, n1, IS2, n2, RS1, I0)\r\n")
        _T("   I2[ii] = Physics.current(V[ii], T, ISC, RP2, IS1, n1, IS2, n2, RS1, I0)\r\n")
        _T("   if (I1[ii] >= 0) and (I2[ii] >= 0) then\r\n")
        _T("      break\r\n")
        _T("   end\r\n")
        _T("end\r\n")
        _T("tD:set(\"A\", V)\r\n")
        _T("tD:set(\"B\", I1)\r\n")
        _T("tD:set(\"C\", I2)\r\n\r\n")

        _T("tP = Plot:create()\r\n")
        _T("tP:add(\"A\", \"B\", 1)\r\n")
        _T("tP:add(\"A\", \"C\", 1)\r\n")
        _T("tP:axis(1, 1, 1, 0, 0, 1, \"Voltage(V)\")\r\n")
        _T("tP:axis(2, 1, 1, 0, 0, 1, \"Current(A)\")\r\n")
        _T("tP:color(1, 255, 0, 0)\r\n")
        _T("tP:color(2, 0, 0, 255)\r\n")
        _T("tP:update()\r\n\r\n")

        _T("print(toc())    -- Print the elapsed time(sec.) since tic was used.\r\n")
    }

};
