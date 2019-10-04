#!/bin/sh
width=1920
height=1080
camera_apartment_0="-0.707107 0 0.707107 1.73464 0 1 0 -0.2 -0.707107 0 -0.707107 5.27308 0 0 0 1"
#0.980785 0 -0.19509 -0.142928 0 1 0 -0.3 0.19509 0 0.980785 0.713211 0 0 0 1
camera_apartment_1="0.55557 -9.91189e-08 0.83147 1.62652 0 1 1.19209e-07 -0.100137 -0.83147 -6.62291e-08 0.55557 -1.95495 0 0 0 1"
#"0.55557 -0.31819 0.768178 0.720356 0 0.92388 0.382684 0 -0.83147 -0.212608 0.51328 -1.98927 0 0 0 1"
camera_apartment_2="0.382683 0.18024 -0.906128 2.7437 0 0.980785 0.19509 0.2 0.92388 -0.0746578 0.37533 -5.83001 0 0 0 1"
camera_frl_apartment_0="0.831469 -0.308659 0.46194 4.85363 0 0.831469 0.55557 0.8 -0.555571 -0.46194 0.691341 7.80303 0 0 0 1"
camera_frl_apartment_1="-0.980785 -0.0746579 0.18024 3.61305 0 0.923879 0.382684 0.0999998 -0.19509 0.375331 -0.906127 -2.54595 0 0 0 1"
#"0.707107 0.392848 -0.587938 1.58377 0 0.831469 0.55557 0.5 0.707107 -0.392848 0.587938 -1.20245 0 0 0 1"
camera_frl_apartment_2="-1 0 0 2.96756 0 0.923879 0.382684 -0.2 0 0.382684 -0.923879 -5.27085 0 0 0 1"
camera_frl_apartment_3="0.92388 0.212608 -0.31819 2.45846 0 0.83147 0.55557 0.4 0.382684 -0.51328 0.768178 -0.151309 0 0 0 1"
camera_frl_apartment_4="-0.555571 0.162212 -0.815493 4.37525 0 0.980785 0.195091 -0.6 0.831469 0.108387 -0.544896 5.20065 0 0 0 1"
camera_frl_apartment_5="-0.19509 -0.37533 0.906127 5.41406 0 0.92388 0.382683 -0.3 -0.980785 0.0746578 -0.18024 -2.80907 0 0 0 1" 
camera_hotel_0="0.923879 -3.42146e-08 0.382684 1.16429 0 1 8.9407e-08 0.5 -0.382684 -8.26013e-08 0.923879 0.951958 0 0 0 1" 
#"-0.555571 -0.162212 0.815493 2.23577 0 0.980785 0.19509 0.5 -0.831469 0.108387 -0.544896 -1.38661 0 0 0 1"
camera_office_0="0.55557 0 0.83147 1.55437 0 1 0 -0.2 -0.83147 0 0.55557 2.04006 0 0 0 1" 
camera_office_1="0.980785 0.0746578 -0.18024 0.00960542 0 0.92388 0.382684 0.8 0.19509 -0.37533 0.906127 0.912989 0 0 0 1" 
camera_office_2="-0.195091 -0.191342 0.96194 2.45131 0 0.980785 0.195091 0.3 -0.980785 0.0380604 -0.191342 -3.06157 0 0 0 1" 
camera_office_3="0.83147 0.212608 -0.51328 -3.07453 0 0.92388 0.382683 0.5 0.55557 -0.31819 0.768178 4.31459 0 0 0 1" 
camera_office_4="-0.831469 1.65573e-08 -0.55557 0.638562 0 1 2.98023e-08 0.3 0.55557 2.47797e-08 -0.831469 -4.07735 0 0 0 1" 
camera_room_0="5.96046e-08 0.19509 -0.980785 0.0628238 0 0.980785 0.19509 -0.1 1 -1.16283e-08 5.84594e-08 -1.20446 0 0 0 1"
#"0.382684 0.18024 -0.906127 0.062824 0 0.980785 0.19509 -0.1 0.92388 -0.0746578 0.37533 0.295537 0 0 0 1" 
camera_room_1="0.19509 -0.37533 0.906127 0.346842 0 0.923879 0.382684 0.0999999 -0.980785 -0.0746579 0.18024 -0.109684 0 0 0 1"
camera_room_2="0.195091 -3.36141e-07 0.980785 5.74009 0 1 3.42727e-07 -1.3 -0.980785 -6.68628e-08 0.195091 1.35675 0 0 0 1"
for i in apartment_0 apartment_1 apartment_2 frl_apartment_0 frl_apartment_1 frl_apartment_2 frl_apartment_3 frl_apartment_4 frl_apartment_5 hotel_0 office_0 office_1 office_2 office_3 office_4 room_0 room_1 room_2
do 
eval "cam=\${camera_${i}}"
echo python examples/example.py --scene ~/models/replica/$i/habitat/mesh_semantic.ply --save_png --width $width --height $height --max_frames 1 --sensor_height 0 --camera $cam
python examples/example.py --scene ~/models/replica/$i/habitat/mesh_semantic.ply --save_png --width $width --height $height --max_frames 1 --sensor_height 0 --camera $cam 
mv test.rgba.00000.png $i.before.${height}p.png
echo python examples/example.py --scene ~/models/replica/$i/mesh.ply --save_png --width $width --height $height --max_frames 1 --sensor_height 0 --camera $cam
python examples/example.py --scene ~/models/replica/$i/mesh.ply --save_png --width $width --height $height --max_frames 1 --sensor_height 0 --camera $cam 
mv test.rgba.00000.png $i.after.${height}p.png
done
#python examples/example.py --scene ~/models/replica/apartment_1/habitat/mesh_semantic.ply --save_png --width 1920 --height 1080 --max_frames 1 --sensor_height 0 --camera $apartment_1_camera


