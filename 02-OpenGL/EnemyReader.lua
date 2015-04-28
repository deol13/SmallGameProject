function errhandler(errmsg) 
	return errmsg .. "\n" .. debug.traceback()
end
	
function readFile()
	enemyArr = {}
	file = io.open("AltWave.dat", "r")	-- Opens a file in read
	io.input(file)							-- sets the default input file as test.lua

	if waveNr == nil then waveNr = 1 end
	
	tempTester = "x" .. waveNr
	temp = io.read("*l")
	
	while temp ~= tempTester				--Finds the correct wave that we are searching for
	do
		temp = io.read("*l")
	end

	nrOfEnemies = io.read("*l")				-- prints the first line of the file then moves the pointer to the next line 
	nrOfEnemies = tonumber(nrOfEnemies)
	if nrOfEnemies ~= nil or nrOfEnemies ~= 0 --Is there any meaning to doing the rest?
	then
		c = nrOfEnemies * 4	
		
		for i = 1, c , 4
		do
			enemyType = io.read("*l")	
			if enemyType == "melee"			-- decide type
			then
				enemyArr[i] = 1
				enemyArr[i+3] = 1
			else
				enemyArr[i] = 1
				enemyArr[i+3] = 1
			end

			enemyArr[i + 1] = io.read("*l")		--X
			--enemyArr[i + 1] = tonumber(modelArr[i + 1])
			enemyArr[i + 2] = io.read("*l")		--Z
			--enemyArr[i + 2] = tonumber(modelArr[i + 2])
			
		end
	end		
	
	io.close(file)							-- closes the open file

	return enemyArr, nrOfEnemies
end
