FindContours: FindContours.cpp
	g++ -L/usr/lib/uv4l/uv4lext/armv6l/ -I ../src/ -I WPILib/ -L ./ -L WPILib/ -lraspicam -luv4lext -Wl,-rpath,'/usr/lib/uv4l/uv4lext/armv6l',-rpath,'/usr/local/lib' `pkg-config --cflags opencv` `pkg-config --libs opencv` -o FindContours FindContours.cpp -rdynamic '/home/pi/Paladins/networktables-pc/build/libnetworktables_static.a'
	@echo Execute with LD_PRELOAD=\"./libraspicam.so.0.0\" ./FindContours

