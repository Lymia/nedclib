Nintendo eReader Dotcode encoder/decoder
Copyright (C) 2007 by CaitSith2

This is a command line tool. As such, basic command line usage is assumed. if you don't
know how to use the command line, then you should look up how.

Usage is "nedcenc [options]"

[options]
		-i <Input file>		(Required)
		-o <Output file>
		
		-e					Encodes bin dotcode files to raw (-o option required) (default)
		-d					Decodes raw dotcode files to bin (-o option required)
		-f					Fixes dotcode raw files.
		
		-s <Signature>		There are either 40 or 44 bytes unused in a dotcode raw
							file, depending on if the raw is for a long dotcode or
							short dotcode. You can input any string or hex value.
							to input spaces, quote the string or use <20>. To insert
							quotes, use <22>. To insert <, use << or <3C>.
							To input any hex string, start with <. then input the
							hex string, then end with >.
							Example.
							
							"<<Dotcode encoded by> -=<<CaitSith2><65027110>"
							
							If -s is not specified, dotcodes will be filled with either
							<2425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F>
							<404142434445464748494A4B4C4D4E4F> (short)
							or
							<38393A3B3C3D3E3F404142434445464748494A4B4C4D4E4F50515253>
							<5455565758595A5B5C5D5E5F> (long).
		
		

Release history
v1.0
	-Initial release.
	-(No longer need to use VBA e-reader for encoding needs)
	
v1.1
	-Added signature option
	-Encoding bin2raw is now default operation.

v1.2
	-Functions ported to nedclib.dll.  (Source code available seperately)