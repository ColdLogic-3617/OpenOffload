# /tmp folder is cleaned out each boot cycle
mkdir /tmp/stream
# Our program could go here.  Something needs to create an image
# in /tmp/stream/pic.jpg, as that is what will be sent for http request
raspistill --nopreview -w 640 -h 480 -q 5 -o /tmp/stream/pic.jpg -tl 100 -t 9999999 -th 0:0:0 &
# start the http server, specifying the reseved, normal http port (80).
sudo LD_LIBRARY_PATH=/usr/local/lib mjpg_streamer -i "input_file.so -f /tmp/stream -n pic.jpg" -o "output_http.so -w /usr/local/www -port 80"
