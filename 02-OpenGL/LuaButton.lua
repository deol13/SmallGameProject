     b = button{label = "Hello world!" command = "exit(0)"}
	 
	 w = toplevel{b}
     w:show()

	      b1 = button{label="Yes", command="yes=1"}
     b2 = button{label="No", command="yes=0"}
     w  = toplevel{
					message{text="Overwrite file?"},
                    frame{b1, b2; side="left"};
                    side="top"
				   }