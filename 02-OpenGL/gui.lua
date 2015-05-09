startMenu =     { -1.0, 1.0, 1.0, -1.0, 0, 0, 1, 1, 0,         --Background
			      -0.95, -0.497, -0.61, -0.924, 0, 0, 1, 1, 1, --New Game
			      -0.56, -0.497, -0.22, -0.924, 0, 0, 1, 1, 2, --Continue
			      -0.17, -0.497,  0.17, -0.924, 0, 0, 1, 1, 3, --How to play
			       0.22, -0.497,  0.56, -0.924, 0, 0, 1, 1, 4, --Map creation
			       0.61, -0.497,  0.95, -0.924, 0, 0, 1, 1, 5 } -- Exit


howToPlayMenu = { -1.0, 1.0, 1.0, -1.0, 0, 0, 1, 1, 6,      --How to play text
				  -0.841, -0.475, -0.441, -0.875, 0, 0, 1, 1, 7 } --Back

				  
pauseMenu =		{ -0.312, 0.25, 0.312,  0.0 , 0, 0, 1, 1, 8,  --Resume
				  -0.312, 0.0 , 0.312, -0.25, 0, 0, 1, 1, 9 } --Quit

stadardInGameGui = { -1.0, 1.0, 1.0, 0.848, 0, 0, 1, 1, 0,          --Background

					 -0.313, 0.987, 0.0  , 0.861, 0, 0, 1, 1, 5,
					 -0.0  , 0.987, 0.313, 0.861, 0, 0, 1, 1, 6, --Sword and spear 

					 0.825, 0.973, 0.950, 0.751,   0, 0, 1, 1, 8, --Combo

				     -0.950, 0.979, -0.8875, 0.869, 0, 0, 1, 1, 1, --Heart
				     -0.8795, 0.979, -0.817, 0.869, 0, 0, 1, 1, 1, --Heart
				     -0.809, 0.979, -0.7465, 0.869, 0, 0, 1, 1, 1 } --Heart 
					 
					 -- -0.7382, 0.979, -0.6757, 0.869, 0, 0, 1, 1, 1,
					 -- -0.6674, 0.979, -0.6049, 0.869, 0, 0, 1, 1, 1,
					 -- -0.5966, 0.979, -0.5341, 0.869, 0, 0, 1, 1, 1 }

					 -- 0.021= 15p  0.014 = 10p   0.11 = 80p  0.0625   0.008
					 -- 0.027 = 20p  0.222 = 160p    0.125 = 160p

					 -- -0.5258, 0.972, -0.2128 , 0.750, 0, 0, 1, 1, 4,
					 -- -0.2045, 0.972, 0.1085 , 0.750, 0, 0, 1, 1, 5 } --Sword and spear


heartOne = { -0.7382, 0.979, -0.6757, 0.869, 0, 0, 1, 1, 1 }    --Heart
heartTwo = { -0.6674, 0.979, -0.6049, 0.869, 0, 0, 1, 1, 1 }    --Heart
heartThree = { -0.5966, 0.979, -0.5341, 0.869, 0, 0, 1, 1, 1 }  --Heart

empty = {}

maxHearts = 6
currentHearts = 3

function init() --[[ When the game starts, lua returns how the start menu should look like ]]
	return startMenu, 1
end


function clicked( xPos, yPos, state ) --[[ When the player clicks somewhere in a menu state ]]
	if state == 1 then  --[[ Start menu ]]
		if xPos < -0.61 and xPos > -0.95  and yPos < -0.497 and yPos > -0.924 then
			stateChange(1) --[[ New game ]]
		--[[ elseif xPos < -0.22 and xPos > -0.56  and yPos < -0.497 and yPos > -0.924 then
			stateChange(1) ]] --[[ Continue game ]]
		elseif xPos < 0.17 and xPos > -0.17 and yPos < -0.497 and yPos > -0.924 then
			return howToPlayMenu, 2
		--[[ elseif xPos < 0.56 and xPos > 0.22  and yPos < -0.497 and yPos > -0.924 then
			stateChange(2) ]] --[[ Map Creation ]]
		elseif xPos < 0.95 and xPos > 0.61 and yPos < -0.497 and yPos > -0.924 then
			stateChange(3) --[[ Exit game ]]
		else
			return -1 --[[ Empty space clicked on ]]
		end
	elseif state == 2 then --[[ Getting back to the startMenu from "howToPlayMenu" ]]
		if xPos < -0.441 and xPos > -0.841 and yPos < -0.475 and yPos > -0.875 then
			return startMenu, 1
		end
	elseif state == 3 then 
		if xPos < 0.312 and xPos > -0.312  and yPos < 0.25 and yPos > 0.0 then          --[[Resume]]
			stateChange(0)
		elseif xPos < 0.312 and xPos > -0.312  and yPos < 0.0 and yPos > -0.25 then     --[[Exit to start menu]]
			stateChange(2)
		end
	else
		return -1
	end
	
	return -1
end


function pauseGame()
	return pauseMenu, 3
end

function inGameGUI()
	return stadardInGameGui
end

function addHeart()
	if currentHearts == 3 then
		currentHearts = currentHearts + 1
		return heartOne
	elseif currentHearts == 4 then
		currentHearts = currentHearts + 1
		return heartTwo
	elseif currentHearts == 5 then
		currentHearts = currentHearts + 1
		return heartThree
	else
		return empty
	end
end