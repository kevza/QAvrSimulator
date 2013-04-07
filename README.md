QAvrSimulator
=============

This is an Atmega AVR simulator.

Key Features:
	-Plugin framework for adding extra features and hardware
	-Graphical interface for input and output
	-Debug window to see what is happeing in the simulated chip
	-Runs AVR ihex files by way of assembly simulator

To Build:
	-First build the QAvrCore project, this will create the a static library for the simulated core.
	-Build QAvrRunner (The gui), this will need to be linked to the library created by building QAvrCore
	-Build the relavant plugins (QAvrTimers 16B, QAvrLedMat, QAvrIO), create a plugins directory in the QAvrRunner build directory (Where QAvrRunner will run) and then copy the plugin libraries you
	have just created into this directory (.so files underlinux)
	
To Run:
	-Make sure there is a directory called plugins in the working directory for QAvrRunner
	also make sure this directory has the required plugins as mentioned above.
	-QAvrRunner needs definition files that describe what plugins to load, in the repository there are demo .def files that use the 
	required plugins, copy then def files into the plugins directory.
	//Note for windows and mac users, the def files will need editing on your systems. Open the def files in a text editor and change the library file names to match what 
	your system produces. Also mac users may have some trouble with the program finding the path to plugins dir, I intend to fix this at some point but I don't have a mac.
	-Run QAvrSimulator and load a hex file to test.

Note:
	-This program is still very much under construction and advanced atmega programs will not run yet.

Features not implemented:
	-Most interrupts
	-8 Bit timer*
	-Uart*
	-Spi
	//Im working on these as fast as time allows, at this point I do not plan to implement full interrupt functionality, features marked with * are top of the priority list
	//features like pwm ect are probably not going to happen.
