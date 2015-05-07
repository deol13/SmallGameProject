startMenu = { -0.25, 1.0, 0.25, 0.5, 0, 0, 1, 1, 0,
			  -0.25, 0.5, 0.25, 0.0, 0, 0, 1, 1, 1,
			  -0.25, 0.0, 0.25, -0.5, 0, 0, 1, 1, 2,
			  -0.25, -0.5, 0.25, -1.0, 0, 0, 1, 1, 3
			   }

howToPlayMenu = { -0.5, 1.0, 0.5, 0, 0, 0, 1, 1, 4,
				  -0.5, 0, -0.10, -0.40, 0, 0, 1, 1, 5 }

pauseMenu =		{ -0.312, 0.25, 0.312,  0.0 , 0, 0, 1, 1, 6, 
				  -0.312, 0.0 , 0.312, -0.25, 0, 0, 1, 1, 7 }

function init() --[[ When the game starts, lua returns how the start menu should look like ]]
	return startMenu, 1
end


function clicked( xPos, yPos, state ) --[[ When the player clicks somewhere in a menu state ]]
	if state == 1 then  --[[ Start menu ]]
		if xPos < 0.25 and xPos > -0.25  and yPos < 1.0 and yPos > 0.5 then
			stateChange(1) --[[ New game ]]
		--[[ elseif xPos < 0.25 and xPos > -0.25  and yPos < 0.5and yPos > 0.0 then
			stateChange(1) ]] --[[ Continue game ]]
		elseif xPos < 0.25 and xPos > -0.25  and yPos < 0.0 and yPos > -0.5 then
			return howToPlayMenu, 2
		elseif xPos < 0.25 and xPos > -0.25 and yPos < -0.5 and yPos > -1.0 then
			stateChange(2) --[[ Exit game ]]
		else
			return -1 --[[ Empty space clicked on ]]
		end
	elseif state == 2 then --[[ Getting back to the startMenu from "howToPlayMenu" ]]
		if xPos < -0.10 and xPos > -0.5 and yPos < 0.0 and yPos > -0.40 then
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

function healthLoss( health )
	health = health - 1
	return health
end

function goldChanged( gold, value )
	gold = gold + value
	return gold
end