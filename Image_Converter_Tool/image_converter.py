from PIL import Image
from sys import argv
import os 
import glob
import collections

array_name = str(argv[1])

output = ""


def convert(image):
    im = Image.open(image)
    trans_data = []
    data_out = ""
    for y in range(0, im.size[1]):
        for x in range(0, im.size[0]):
            r, g, b, a = im.getpixel((x,y))
            if a > 0:
                trans_data.append(1)
            else:
                trans_data.append(0)
    byte_count = 7
    byte_string = ''
    byte_array = []
    for i in range(0, len(trans_data)):
        if trans_data[i] == 1:
            byte_string = byte_string + '1'
        else:
            byte_string = byte_string + '0'
        byte_count = byte_count - 1
        if byte_count < 0:
            byte_array.append(byte_string)
            byte_string = ''
            byte_count = 7
    hex_array = "\t"
    ft_counter = 15
    for i in range(0, len(byte_array)):
        hex_array = hex_array + "0x{:02X}, ".format((int(byte_array[i], 2))) 
        ft_counter = ft_counter -1
        if ft_counter < 0:
            ft_counter = 15
            hex_array += "\n"
            hex_array += "\t"
    data_out += hex_array + "\n"
    return data_out
    img_width = im.size[0]
    im.close()
    
def batch_convert(images):
    data_out = ""
    cnt = 0
    for image in images:
        with open(image, 'rb') as file:
            data_out += "//Frame {:d}\n".format(cnt)
            data_out += convert(file)
            cnt += 1
    return data_out


images = glob.glob('*.png')

s_im = Image.open(images[0])
im_width, im_height = s_im.size
s_im.close()

with open("delay.txt") as f:
    delay_data = f.read().splitlines()

delay_string = ""    
for i in range(0, len(images)):
    delay_string += "{}, ".format(delay_data[i])

output += "#include <Arduino.h>\n\n"
output += "#ifndef {}_H\n".format(array_name.upper())
output += "#define {}_H\n\n".format(array_name.upper())
output += "//Format - Width, Height, Frame Count, Frame Delay 0, Frame Delay 1...\n"
output += "const uint16_t {}_header[] PROGMEM".format(array_name) + "{ " + "{0}, {1}, {2}".format(im_width, int(im_height/8), len(images)) + ", " + delay_string + "};\n\n"
output += ("const uint8_t {}_data[] PROGMEM  = ".format(array_name) + "{\n")      
output += batch_convert(images)
output += "};\n\n"
output += "#endif"


f = open("{}.h".format(array_name), "w+")
f.write(output)
f.close()
print(output)
