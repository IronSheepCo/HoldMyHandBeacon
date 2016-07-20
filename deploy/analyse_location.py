from Tkinter import *
import sys
import select
import re

root = Tk()

delay = 40
beacon_radius = 5
room_width = 500
room_height = 500

beacon_drawing = {}

def add_beacon(info):
    text_to_show = "Beacon: "+info("hash")+" tx: "+info("tx")+" x: "+info("x")+" y: "+info("y")
    label = Label(top_frame, text=text_to_show)
    label.pack()
    
    #draw the beacon, just do it
    x = int( info("x") )
    y = int( info("y") )
    #flip y
    y = room_width-y
    beacon_circle = room.create_oval( x-beacon_radius, y-beacon_radius, x+beacon_radius, y+beacon_radius ) 

def read_input():

    while sys.stdin in select.select( [sys.stdin], [], [], 0)[0]:
        line = sys.stdin.readline()
        if line:
            mo = re.match( "Found new beacon hash: (?P<hash>-?[0-9]+), tx_power: (?P<tx>-?[0-9]+), pos_x: (?P<x>-?[0-9]+),  pos_y: (?P<y>-?[0-9]+)", line)
            if mo:
                print( mo.group("hash")+mo.group("tx")+mo.group("x")+mo.group("y") )
                add_beacon( mo.group )
        else:
            print("end of input")
    
    root.after(delay, read_input)

top_frame = Frame(root)
top_frame.pack()

label = Label(top_frame, text="Analysing data ....")
label.pack()

bottom_frame = Frame(root)
bottom_frame.pack()

label = Label(bottom_frame, text="Beacons position")
label.pack()

room = Canvas(bottom_frame, width=room_width, height=room_height)
room.pack()

root.after(delay, read_input)
root.mainloop()
