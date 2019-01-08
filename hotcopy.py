#!/usr/bin/env python
import string
import sys
import os
import shutil

if (len(sys.argv) <= 1):
  print("You must supply a filename")
  exit()
  
buildpath = '/home/havenbuild/area/'
livepath = '/home/havenlive/area/'
areaname = sys.argv[1]

if (os.path.exists(buildpath + areaname) == False):
  print('Source does not exist.. exiting.')
  exit()
  
if (os.path.exists(livepath + areaname) == False):
  print('New area, adding to area.lst.')
  file = open("area.lst", "r")
  lines = file.readlines() 
  file.close()
  lines.insert(-1, areaname + '\n')
  file = open("area.lst", "w")
  file.writelines(lines)
  file.close()
  
print('Copying area file..')
shutil.copy(buildpath + areaname, livepath + areaname)

print('Done!')
