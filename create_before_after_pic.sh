#!/bin/sh
width=1920
height=1080
apartment_1_camera="0.55557 -0.31819 0.768178 0.720356 0 0.92388 0.382684 0 -0.83147 -0.212608 0.51328 -1.98927 0 0 0 1"
#for i in apartment_0 apartment_1 apartment_2 frl_apartment_0 frl_apartment_1 frl_apartment_2 frl_apartment_3 frl_apartment_4 frl_apartment_5 hotel_0 office_0 office_1 office_2 office_3 office_4 room_0 room_1 room_2
for i in apartment_1
do 
eval "cam=\${${i}_camera}"
echo python examples/example.py --scene ~/models/replica/$i/habitat/mesh_semantic.ply --save_png --width $width --height $height --max_frames 1 --sensor_height 0 --camera $cam
python examples/example.py --scene ~/models/replica/$i/habitat/mesh_semantic.ply --save_png --width $width --height $height --max_frames 1 --sensor_height 0 --camera $cam 
mv test.rgba.00000.png $i.before.${height}p.png
echo python examples/example.py --scene ~/models/replica/$i/mesh.ply --save_png --width $width --height $height --max_frames 1 --sensor_height 0 --camera $cam
python examples/example.py --scene ~/models/replica/$i/mesh.ply --save_png --width $width --height $height --max_frames 1 --sensor_height 0 --camera $cam 
mv test.rgba.00000.png $i.after.${height}p.png
done
#python examples/example.py --scene ~/models/replica/apartment_1/habitat/mesh_semantic.ply --save_png --width 1920 --height 1080 --max_frames 1 --sensor_height 0 --camera $apartment_1_camera


