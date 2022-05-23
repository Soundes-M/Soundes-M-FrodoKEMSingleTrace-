//------------------------------------------------------------------------------
//
// mac16_generic.v
// -----------------------------------------------------------------------------
// Generic 16-bit multiplier and 47-bit accumulator.
//
// Authors: Pavel Shatov
//
// Copyright (c) 2016, NORDUnet A/S
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// - Neither the name of the NORDUnet nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//------------------------------------------------------------------------------

module mac16_generic
	(
		input					clk,		// clock
		input					clr,		// clear accumulator (active-high)
		input					ce,		// enable clock (active-high)
		input		[15: 0]	a,			// operand input
		input		[15: 0]	b,			// operand input
		output	[46: 0]	s			// sum output
	);
	
		//
		// Multiplier
		//
	wire	[31: 0]	p = {{16{1'b0}}, a} * {{16{1'b0}}, b};
	wire	[46: 0]	p_ext = {{15{1'b0}}, p};
		
		//
		// Accumulator
		//
	reg	[46: 0]	s_int;
	
	always @(posedge clk)
		//
		if (ce) s_int <= clr ? p_ext : p_ext + s_int;
		
		//
		// Output
		//
	assign s = s_int;

endmodule


//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
