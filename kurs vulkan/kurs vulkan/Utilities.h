#pragma once

//indices (location) of queue families (if they exist)

struct QueueFamilyIndices {

	int graphicsFamily = -1; //location of graphics queue family

	//check if valids
	bool isValid() {
		return graphicsFamily >= 0;
	}

};
