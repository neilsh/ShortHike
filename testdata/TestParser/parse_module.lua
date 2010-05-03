Module{
   identifier = "TestParser/parse_module",
   name = "parse_module",
   mesh = "parse_module.mesh",
   icon = "parse_module.png",

   ports = {
      {
	 name = "fore",
	 position = {
	    x = 0, y = 0, z = -12
	 },
	 orientation = {
	    {
	       axis_x = 0, axis_y = 1, axis_z = 0, angle = 180
	    },
	 },
      },

      {
	 name = "aft",
	 position = {
	    x = 0, y = 0, z = 12
	 },
	 orientation = {
	 },
      },
   },

   simulation = {
      energy = {
	 output = 20,
	 flow = 30
      };

      thermal = {
	 input = 21,
	 flow = 31
      };

      lifesupport = {
	 output = 22,
	 flow = 32
      };

      crew = {
	 input = 10
      };
   };
}

