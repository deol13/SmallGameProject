function errhandler(errmsg) 
	return errmsg .. "\n" .. debug.traceback()
end
	
function readFile()
	arenaArr = {}
	file = io.open("Arena.dat", "r")	-- Opens a file in read
	io.input(file)							-- sets the default input file as test.lua

	if arenaNr == nil then arenaNr = 1 end
	
	tempTester = "x" .. arenaNr
	temp = io.read("*l")
	
	while temp ~= tempTester				--Finds the correct wave that we are searching for
	do
		temp = io.read("*l")
	end

	nrOfObjects = io.read("*l")				-- prints the first line of the file then moves the pointer to the next line 
	nrOfObjects = tonumber(nrOfObjects)
	if nrOfObjects ~= nil or nrOfObjects ~= 0 --Is there any meaning to doing the rest?
	then
		c = nrOfObjects * 5	
		
		for i = 1, c , 1
		do
			arenaArr[i] = io.read("*l")	
			if i % 5 == 2
			then
				arenaArr[i] = findTexture(arenaArr[i])
			end
		end
	end		
	
	io.close(file)							-- closes the open file

	return arenaArr, nrOfObjects
end

function findTexture(name)			-- Matches the texture names to their GLuint
	if name == "floor.png"
	then
		return 4
	else
		return 2
	end
end
